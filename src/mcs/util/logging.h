//
// Created by lianyu on 2023/1/4.
//

#pragma once

#include <atomic>
#include <chrono>
#include <functional>
#include <iostream>
#include <memory>
#include <sstream>
#include <string>
#include <vector>

namespace mcs {
  class StackTrace {
    /// dump 时打印调用栈，目前没做.
    friend std::ostream &operator<<(std::ostream &os, const StackTrace &stack_trace);
  };

  enum class McsLogLevel {
    TRACE = -2,
    DEBUG = -1,
    INFO = 0,
    WARNING = 1,
    ERROR = 2,
    FATAL = 3
  };

#define MCS_LOG_INTERNAL(level) ::mcs::McsLog(__FILE__, __LINE__, level)

#define MCS_LOG_ENABLED(level) mcs::McsLog::IsLevelEnabled(mcs::McsLogLevel::level)

#define MCS_LOG(level)                                      \
  if (mcs::McsLog::IsLevelEnabled(mcs::McsLogLevel::level)) \
  MCS_LOG_INTERNAL(mcs::McsLogLevel::level)

#define MCS_IGNORE_EXPR(expr) ((void)(expr))

#define MCS_CHECK(condition)                                                          \
  (condition)                                                                         \
      ? MCS_IGNORE_EXPR(0)                                                            \
      : ::mcs::Voidify() & ::mcs::McsLog(__FILE__, __LINE__, mcs::McsLogLevel::FATAL) \
                               << " Check failed: " #condition " "
#ifdef NDEBUG

  #define MCS_DCHECK(condition)                                                         \
  (condition)                                                                         \
      ? MCS_IGNORE_EXPR(0)                                                            \
      : ::mcs::Voidify() & ::mcs::McsLog(__FILE__, __LINE__, mcs::McsLogLevel::ERROR) \
                               << " Debug check failed: " #condition " "
#else

#define MCS_DCHECK(condition) MCS_CHECK(condition)

#endif  // NDEBUG

  class McsLogBase {
  public:
    virtual ~McsLogBase(){};

    virtual bool IsEnabled() const { return false; };

    virtual bool IsFatal() const { return false; };

    template <typename T>
    McsLogBase &operator<<(const T &t) {
      if (IsEnabled()) {
        Stream() << t;
      }
      if (IsFatal()) {
        ExposeStream() << t;
      }
      return *this;
    }

  protected:
    virtual std::ostream &Stream() { return std::cerr; };
    virtual std::ostream &ExposeStream() { return std::cerr; };
  };

  class McsLog : public McsLogBase {
  public:
    McsLog(const char *file_name, int line_number, McsLogLevel severity);

    virtual ~McsLog();

    /// \return 日志是否开启.
    virtual bool IsEnabled() const;

    virtual bool IsFatal() const;

    /// 开启一个日志线程，只需要执行一次
    ///
    /// \parem appName 应用名.
    /// \param severity_threshold 日志级别.
    /// \param logDir 输出的日志文件. 如果为空，输出到控制台.
    static void StartMcsLog(const std::string &appName,
                            McsLogLevel severity_threshold = McsLogLevel::INFO,
                            const std::string &logDir = "");

    /// 终止日志线程，和 StartMcsLog 是一对
    /// 如果没有执行 StartMcsLog 这个就是空操作
    static void ShutDownMcsLog();

    /// 返回传入到日志级别是否开启
    ///
    /// \param log_level 检测的日志级别.
    /// \return 如果输入的日志级别不低于目前的日志级别就会返回正确.
    static bool IsLevelEnabled(McsLogLevel log_level);

    static std::string GetLogFormatPattern();

    static std::string GetLoggerName();

  private:
    // 用 void * 隐藏 log provider 的实现，否则可能会产生冲突
    void *logging_provider_;
    /// True: 需要被打印日志
    bool is_enabled_;
    /// 是否是崩溃
    bool is_fatal_ = false;
    /// log level.
    McsLogLevel severity_;
    /// 目前无用
    /// String stream of exposed log content.
    std::shared_ptr<std::ostringstream> expose_osstream_ = nullptr;
    /// Whether or not the log is initialized.
    static std::atomic<bool> initialized_;
    static McsLogLevel severity_threshold_;

    /** 目前以下部分都没用 **/
    static std::string app_name_;
    /// The directory where the log files are stored.
    /// If this is empty, logs are printed to stdout.
    static std::string log_dir_;
    // Log format content.
    static std::string log_format_pattern_;
    // Log rotation file size limitation.
    static long log_rotation_max_size_;
    // Log rotation file number.
    static long log_rotation_file_num_;
    // Mcs default logger name.
    static std::string logger_name_;

  protected:
    virtual std::ostream &Stream();
    virtual std::ostream &ExposeStream();
  };


  // This class make MCS_CHECK compilation pass to change the << operator to void.
  class Voidify {
  public:
    Voidify() {}
    // This has to be an operator with a precedence lower than << but
    // higher than ?:
    void operator&(McsLogBase &) {}
  };

} //namespace mcs