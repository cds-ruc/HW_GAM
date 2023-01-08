//
// Created by lianyu on 2023/1/5.
//

#include "process.h"

#include <poll.h>
#include <signal.h>
#include <stddef.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>

#include <string.h>
#include <array>
#include <algorithm>
#include <atomic>
#include <fstream>
#include <string>
#include <vector>

#include "mcs/util/logging.h"

#ifdef __APPLE__
extern char **environ;

// macOS dosn't come with execvpe.
// https://stackoverflow.com/questions/7789750/execve-with-path-search
int execvpe(const char *program, char *const argv[], char *const envp[]) {
  char **saved = environ;
  int rc;
  // Mutating environ is generally unsafe, but this logic only runs on the
  // start of a worker process. There should be no concurrent access to the
  // environment.
  environ = const_cast<char **>(envp);
  rc = execvp(program, argv);
  environ = saved;
  return rc;
}
#endif

namespace mcs {

  bool EnvironmentVariableLess::operator()(char a, char b) const {
    // TODO(mehrdadn): This is only used on Windows due to current lack of Unicode support.
    // It should be changed when Process adds Unicode support on Windows.
    return std::less<char>()(tolower(a), tolower(b));
  }

  bool EnvironmentVariableLess::operator()(const std::string &a,
                                           const std::string &b) const {
    bool result;
    result = a < b;
    return result;
  }

  class ProcessFD {
    pid_t pid_;
    intptr_t fd_;

  public:
    ~ProcessFD();
    ProcessFD();
    ProcessFD(pid_t pid, intptr_t fd = -1);
    ProcessFD(const ProcessFD &other);
    ProcessFD(ProcessFD &&other);
    ProcessFD &operator=(const ProcessFD &other);
    ProcessFD &operator=(ProcessFD &&other);
    intptr_t CloneFD() const;
    void CloseFD();
    intptr_t GetFD() const;
    pid_t GetId() const;

    // Fork + exec combo. Returns -1 for the PID on failure.
    static ProcessFD spawnvpe(const char *argv[],
                              std::error_code &ec,
                              bool decouple,
                              const ProcessEnvironment &env) {
      ec = std::error_code();
      intptr_t fd;
      pid_t pid;
      ProcessEnvironment new_env;
      for (char *const *e = environ; *e; ++e) {
        MCS_CHECK(*e && **e != '\0') << "environment variable name is absent";
        const char *key_end = strchr(*e + 1 /* +1 is needed for Windows */, '=');
        MCS_CHECK(key_end) << "environment variable value is absent: " << e;
        new_env[std::string(*e, static_cast<size_t>(key_end - *e))] = key_end + 1;
      }
      for (const auto &item : env) {
        new_env[item.first] = item.second;
      }
      std::string new_env_block;
      for (const auto &item : new_env) {
        new_env_block += item.first + '=' + item.second + '\0';
      }
      std::vector<char *> new_env_ptrs;
      for (size_t i = 0; i < new_env_block.size(); i += strlen(&new_env_block[i]) + 1) {
        new_env_ptrs.push_back(&new_env_block[i]);
      }
      new_env_ptrs.push_back(static_cast<char *>(NULL));
      char **envp = &new_env_ptrs[0];

      // TODO(mehrdadn): Use clone() on Linux or posix_spawnp() on Mac to avoid duplicating
      // file descriptors into the child process, as that can be problematic.
      int pipefds[2];  // Create pipe to get PID & track lifetime
      if (pipe(pipefds) == -1) {
        pipefds[0] = pipefds[1] = -1;
      }
      pid = pipefds[1] != -1 ? fork() : -1;
      if (pid <= 0 && pipefds[0] != -1) {
        close(pipefds[0]);  // not the parent, so close the read end of the pipe
        pipefds[0] = -1;
      }
      if (pid != 0 && pipefds[1] != -1) {
        close(pipefds[1]);  // not the child, so close the write end of the pipe
        pipefds[1] = -1;
      }
      if (pid == 0) {
        // Child process case. Reset the SIGCHLD handler.
        signal(SIGCHLD, SIG_DFL);
        // If process needs to be decoupled, double-fork to avoid zombies.
        if (pid_t pid2 = decouple ? fork() : 0) {
          _exit(pid2 == -1 ? errno : 0);  // Parent of grandchild; must exit
        }
        // This is the spawned process. Any intermediate parent is now dead.
        pid_t my_pid = getpid();
        if (write(pipefds[1], &my_pid, sizeof(my_pid)) == sizeof(my_pid)) {
          execvpe(
                  argv[0], const_cast<char *const *>(argv), const_cast<char *const *>(envp));
        }
        _exit(errno);  // fork() succeeded and exec() failed, so abort the child
      }
      if (pid > 0) {
        // Parent process case
        if (decouple) {
          int s;
          (void)waitpid(pid, &s, 0);  // can't do much if this fails, so ignore return value
          int r = read(pipefds[0], &pid, sizeof(pid));
          (void)r;  // can't do much if this fails, so ignore return value
        }
      }
      // Use pipe to track process lifetime. (The pipe closes when process terminates.)
      fd = pipefds[0];
      if (pid == -1) {
        ec = std::error_code(errno, std::system_category());
      }
      return ProcessFD(pid, fd);
    }
  };

  ProcessFD::~ProcessFD() {
    if (fd_ != -1) {
      bool success;
      success = close(static_cast<int>(fd_)) == 0;
      MCS_CHECK(success) << "error " << errno << " closing process " << pid_ << " FD";
    }
  }

  ProcessFD::ProcessFD() : pid_(-1), fd_(-1) {}

  ProcessFD::ProcessFD(pid_t pid, intptr_t fd) : pid_(pid), fd_(fd) {
    if (pid != -1) {
      bool process_does_not_exist = false;
      std::error_code error;
      if (kill(pid, 0) == -1 && errno == ESRCH) {
        process_does_not_exist = true;
      }
      // Don't verify anything if the PID is too high, since that's used for testing
      if (pid < PID_MAX_LIMIT) {
        if (process_does_not_exist) {
          // NOTE: This indicates a race condition where a process died and its process
          // table entry was removed before the ProcessFD could be instantiated. For
          // processes owned by this process, we should make this impossible by keeping
          // the SIGCHLD signal. For processes not owned by this process, we need to come up
          // with a strategy to create this class in a way that avoids race conditions.
          MCS_LOG(ERROR) << "Process " << pid << " does not exist.";
        }
        if (error) {
          // TODO(mehrdadn): Should this be fatal, or perhaps returned as an error code?
          // Failures might occur due to reasons such as permission issues.
          MCS_LOG(ERROR) << "error " << error << " opening process " << pid << ": "
                         << error.message();
        }
      }
    }
  }

  ProcessFD::ProcessFD(const ProcessFD &other) : ProcessFD(other.pid_, other.CloneFD()) {}

  ProcessFD::ProcessFD(ProcessFD &&other) : ProcessFD() { *this = std::move(other); }

  ProcessFD &ProcessFD::operator=(const ProcessFD &other) {
    if (this != &other) {
      // Construct a copy, then call the move constructor
      *this = static_cast<ProcessFD>(other);
    }
    return *this;
  }

  ProcessFD &ProcessFD::operator=(ProcessFD &&other) {
    if (this != &other) {
      // We use swap() to make sure the argument is actually moved from
      using std::swap;
      swap(pid_, other.pid_);
      swap(fd_, other.fd_);
    }
    return *this;
  }

  intptr_t ProcessFD::CloneFD() const {
    intptr_t fd;
    if (fd_ != -1) {
      fd = dup(static_cast<int>(fd_));
      MCS_DCHECK(fd != -1);
    } else {
      fd = -1;
    }
    return fd;
  }

  void ProcessFD::CloseFD() { fd_ = -1; }

  intptr_t ProcessFD::GetFD() const { return fd_; }

  pid_t ProcessFD::GetId() const { return pid_; }

  Process::~Process() {}

  Process::Process() {}

  Process::Process(const Process &) = default;

  Process::Process(Process &&) = default;

  Process &Process::operator=(Process other) {
    p_ = std::move(other.p_);
    return *this;
  }

  Process::Process(pid_t pid) { p_ = std::make_shared<ProcessFD>(pid); }

  Process::Process(const char *argv[],
                   void *io_service,
                   std::error_code &ec,
                   bool decouple,
                   const ProcessEnvironment &env) {
    (void)io_service;
    ProcessFD procfd = ProcessFD::spawnvpe(argv, ec, decouple, env);
    if (!ec) {
      p_ = std::make_shared<ProcessFD>(std::move(procfd));
    }
  }

  std::error_code Process::Call(const std::vector<std::string> &args,
                                const ProcessEnvironment &env) {
    std::vector<const char *> argv;
    for (size_t i = 0; i != args.size(); ++i) {
      argv.push_back(args[i].c_str());
    }
    argv.push_back(NULL);
    std::error_code ec;
    Process proc(&*argv.begin(), NULL, ec, true, env);
    if (!ec) {
      int return_code = proc.Wait();
      if (return_code != 0) {
        ec = std::error_code(return_code, std::system_category());
      }
    }
    return ec;
  }

  std::string Process::Exec(const std::string command) {
    std::array<char, 128> buffer;
    std::string result;
    std::unique_ptr<FILE, decltype(&pclose)> pipe(popen(command.c_str(), "r"), pclose);
    MCS_CHECK(pipe) << "popen() failed for command: " + command;
    while (fgets(buffer.data(), buffer.size(), pipe.get()) != nullptr) {
      result += buffer.data();
    }
    return result;
  }

  Process Process::CreateNewDummy() {
    pid_t pid = -1;
    Process result(pid);
    return result;
  }

  Process Process::FromPid(pid_t pid) {
    MCS_DCHECK(pid >= 0);
    Process result(pid);
    return result;
  }

  const void *Process::Get() const { return p_ ? &*p_ : NULL; }

  pid_t Process::GetId() const { return p_ ? p_->GetId() : -1; }

  bool Process::IsNull() const { return !p_; }

  bool Process::IsValid() const { return GetId() != -1; }

  std::pair<Process, std::error_code> Process::Spawn(const std::vector<std::string> &args,
                                                     bool decouple,
                                                     const std::string &pid_file,
                                                     const ProcessEnvironment &env) {
    std::vector<const char *> argv;
    for (size_t i = 0; i != args.size(); ++i) {
      argv.push_back(args[i].c_str());
    }
    argv.push_back(NULL);
    std::error_code error;
    Process proc(&*argv.begin(), NULL, error, decouple, env);
    if (!error && !pid_file.empty()) {
      std::ofstream file(pid_file, std::ios_base::out | std::ios_base::trunc);
      file << proc.GetId() << std::endl;
      MCS_CHECK(file.good());
    }
    return std::make_pair(std::move(proc), error);
  }

  int Process::Wait() const {
    int status;
    if (p_) {
      pid_t pid = p_->GetId();
      if (pid >= 0) {
        std::error_code error;
        intptr_t fd = p_->GetFD();
        // There are 3 possible cases:
        // - The process is a child whose death we await via waitpid().
        //   This is the usual case, when we have a child whose SIGCHLD we handle.
        // - The process shares a pipe with us whose closure we use to detect its death.
        //   This is used to track a non-owned process, like a grandchild.
        // - The process has no relationship with us, in which case we simply fail,
        //   since we have no need for this (and there's no good way to do it).
        // Why don't we just poll the PID? Because it's better not to:
        // - It would be prone to a race condition (we won't know when the PID is recycled).
        // - It would incur high latency and/or high CPU usage for the caller.
        if (fd != -1) {
          // We have a pipe, so wait for its other end to close, to detect process death.
          unsigned char buf[1 << 8];
          ptrdiff_t r;
          while ((r = read(fd, buf, sizeof(buf))) > 0) {
            // Keep reading until socket terminates
          }
          status = r == -1 ? -1 : 0;
        } else if (waitpid(pid, &status, 0) == -1) {
          // Just the normal waitpid() case.
          // (We can only do this once, only if we own the process. It fails otherwise.)
          error = std::error_code(errno, std::system_category());
        }
        if (error) {
          MCS_LOG(ERROR) << "Failed to wait for process " << pid << " with error " << error
                         << ": " << error.message();
        }
      } else {
        // (Dummy process case)
        status = 0;
      }
    } else {
      // (Null process case)
      status = -1;
    }
    return status;
  }

  bool Process::IsAlive() const {
    if (p_) {
      return IsProcessAlive(p_->GetId());
    }
    return false;
  }

  void Process::Kill() {
    if (p_) {
      pid_t pid = p_->GetId();
      if (pid >= 0) {
        std::error_code error;
        intptr_t fd = p_->GetFD();
        pollfd pfd = {static_cast<int>(fd), POLLHUP};
        if (fd != -1 && poll(&pfd, 1, 0) == 1 && (pfd.revents & POLLHUP)) {
          // The process has already died; don't attempt to kill its PID again.
        } else if (kill(pid, SIGKILL) != 0) {
          error = std::error_code(errno, std::system_category());
        }
        if (error.value() == ESRCH) {
          // The process died before our kill().
          // This is probably due to using FromPid().Kill() on a non-owned process.
          // We got lucky here, because we could've killed a recycled PID.
          // To avoid this, do not kill a process that is not owned by us.
          // Instead, let its parent receive its SIGCHLD normally and call waitpid() on it.
          // (Exception: Tests might occasionally trigger this, but that should be benign.)
        }
        if (error) {
          MCS_LOG(DEBUG) << "Failed to kill process " << pid << " with error " << error
                         << ": " << error.message();
        }
      } else {
        // (Dummy process case)
        // Theoretically we could keep around an exit code here for Wait() to return,
        // but we might as well pretend this fake process had already finished running.
        // So don't bother doing anything.
      }
    } else {
      // (Null process case)
    }
  }


  pid_t GetParentPID() { return getppid(); }

  pid_t GetPID() {
    return getpid();
  }

  bool IsParentProcessAlive() { return GetParentPID() != 1; }

  bool IsProcessAlive(pid_t pid) {
    if (kill(pid, 0) == -1 && errno == ESRCH) {
      return false;
    }
    return true;
  }

}  // namespace mcs

namespace std {

  bool equal_to<mcs::Process>::operator()(const mcs::Process &x,
                                          const mcs::Process &y) const {
    using namespace mcs;
    return !x.IsNull()
           ? !y.IsNull()
             ? x.IsValid()
               ? y.IsValid() ? equal_to<pid_t>()(x.GetId(), y.GetId()) : false
               : y.IsValid() ? false
                             : equal_to<void const *>()(x.Get(), y.Get())
    : false
    : y.IsNull();
  }

  size_t hash<mcs::Process>::operator()(const mcs::Process &value) const {
    using namespace mcs;
    return !value.IsNull() ? value.IsValid() ? hash<pid_t>()(value.GetId())
                                             : hash<void const *>()(value.Get())
    : size_t();
  }

}  // namespace std