//
// Created by lianyu on 2023/1/4.
//

#include "logging.h"
#include "spdlog/sinks/basic_file_sink.h"
#include "spdlog/sinks/rotating_file_sink.h"
#include "spdlog/sinks/stdout_color_sinks.h"
#include "spdlog/spdlog.h"


namespace mcs {

  McsLogLevel McsLog::severity_threshold_ = McsLogLevel::INFO;
  std::string McsLog::app_name_ = "";
  std::string McsLog::log_dir_ = "";
// Format pattern is 2020-08-21 17:00:00,000 I 100 1001 msg.
// %L is loglevel, %P is process id, %t for thread id.
  std::string McsLog::log_format_pattern_ = "[%Y-%m-%d %H:%M:%S,%e %L %P %t] %v";
  std::string McsLog::logger_name_ = "mcs_log_sink";
  long McsLog::log_rotation_max_size_ = 1 << 29;
  long McsLog::log_rotation_file_num_ = 10;
  std::atomic<bool> McsLog::initialized_ = false;


  std::ostream &operator<<(std::ostream &os, const StackTrace &stack_trace) {
    //todo(异常处理)
    return os;
  }

  inline const char *ConstBasename(const char *filepath) {
    const char *base = strrchr(filepath, '/');
    return base ? (base + 1) : filepath;
  }

  //如果没有写入日志文件，就用这个打印到控制台
  class DefaultStdErrLogger final {
  public:
    std::shared_ptr<spdlog::logger> GetDefaultLogger() { return default_stderr_logger_; }
    static DefaultStdErrLogger &Instance() {
      static DefaultStdErrLogger instance;
      return instance;
    }

  private:
    DefaultStdErrLogger() {
      default_stderr_logger_ = spdlog::stderr_color_mt("stderr");
      default_stderr_logger_->set_pattern(McsLog::GetLogFormatPattern());
    }
    ~DefaultStdErrLogger() = default;
    DefaultStdErrLogger(DefaultStdErrLogger const &) = delete;
    DefaultStdErrLogger(DefaultStdErrLogger &&) = delete;
    std::shared_ptr<spdlog::logger> default_stderr_logger_;
  };


  //封装一下 SpdLog
  class SpdLogMessage final {
  public:
    explicit SpdLogMessage(const char *file,
                           int line,
                           int loglevel,
                           std::shared_ptr<std::ostringstream> expose_osstream)
            : loglevel_(loglevel), expose_osstream_(expose_osstream) {
      stream() << ConstBasename(file) << ":" << line << ": ";
    }

    inline void Flush() {
      auto logger = spdlog::get(McsLog::GetLoggerName());
      if (!logger) {
        logger = DefaultStdErrLogger::Instance().GetDefaultLogger();
      }
      if (expose_osstream_) {
        *expose_osstream_ << "\n*** StackTrace Information ***\n" << mcs::StackTrace();
      }
      logger->log(
              static_cast<spdlog::level::level_enum>(loglevel_), /*fmt*/ "{}", str_.str());
      logger->flush();
    }

    ~SpdLogMessage() {  Flush(); }
    inline std::ostream &stream() { return str_; }



  private:
    SpdLogMessage(const SpdLogMessage &) = delete;
    SpdLogMessage &operator=(const SpdLogMessage &) = delete;

  private:
    std::ostringstream str_;
    int loglevel_;
    std::shared_ptr<std::ostringstream> expose_osstream_;
  };

  typedef mcs::SpdLogMessage LoggingProvider;

  static int GetMappedSeverity(McsLogLevel severity) {
    switch (severity) {
      case McsLogLevel::TRACE:
        return spdlog::level::trace;
      case McsLogLevel::DEBUG:
        return spdlog::level::debug;
      case McsLogLevel::INFO:
        return spdlog::level::info;
      case McsLogLevel::WARNING:
        return spdlog::level::warn;
      case McsLogLevel::ERROR:
        return spdlog::level::err;
      case McsLogLevel::FATAL:
        return spdlog::level::critical;
      default:
        MCS_LOG(FATAL) << "Unsupported logging level: " << static_cast<int>(severity);
        // This return won't be hit but compiler needs it.
        return spdlog::level::off;
    }
  }

  void McsLog::StartMcsLog(const std::string &app_name,
                           McsLogLevel severity_threshold,
                           const std::string &log_dir) {
    severity_threshold_ = severity_threshold;
    app_name_ = app_name;
    log_dir_ = log_dir;

    // All the logging sinks to add.
    std::vector<spdlog::sink_ptr> sinks;
    auto level = static_cast<spdlog::level::level_enum>(severity_threshold_);
    std::string app_name_without_path = app_name;
    if (app_name.empty()) {
      app_name_without_path = "DefaultApp";
    }

    // Format pattern is 2020-08-21 17:00:00,000 I 100 1001 msg.
    // %L is loglevel, %P is process id, %t for thread id.
    log_format_pattern_ =
            "[%Y-%m-%d %H:%M:%S,%e %L %P %t] (" + app_name_without_path + ") %v";
    auto console_sink = std::make_shared<spdlog::sinks::stdout_color_sink_mt>();
    console_sink->set_pattern(log_format_pattern_);
    console_sink->set_level(level);
    sinks.push_back(console_sink);

    // Set the combined logger.
    auto logger = std::make_shared<spdlog::logger>(
            McsLog::GetLoggerName(), sinks.begin(), sinks.end());
    logger->set_level(level);
    logger->set_pattern(log_format_pattern_);
    spdlog::set_level(static_cast<spdlog::level::level_enum>(severity_threshold_));
    spdlog::set_pattern(log_format_pattern_);
    spdlog::set_default_logger(logger);

    initialized_ = true;
  }

  void McsLog::ShutDownMcsLog() {
    if (!initialized_) {
      // If the log wasn't initialized, make it no-op.
      MCS_LOG(INFO) << "The log wasn't initialized. ShutdownMcsLog requests are ignored";
      return;
    }
    if (spdlog::default_logger()) {
      spdlog::default_logger()->flush();
    }
  }

  bool McsLog::IsLevelEnabled(McsLogLevel log_level) {
    return log_level >= severity_threshold_;
  }

  std::string McsLog::GetLogFormatPattern() { return log_format_pattern_; }

  std::string McsLog::GetLoggerName() { return logger_name_; }

  McsLog::McsLog(const char *file_name, int line_number, McsLogLevel severity)
          : logging_provider_(nullptr),
            is_enabled_(severity >= severity_threshold_),
            severity_(severity) {
    if (is_enabled_) {
      logging_provider_ = new LoggingProvider(
              file_name, line_number, GetMappedSeverity(severity), expose_osstream_);
    }
  }

  std::ostream &McsLog::Stream() {
    auto logging_provider = reinterpret_cast<LoggingProvider *>(logging_provider_);
    //如果没开启日志这里会返回空,不会打印出来
    return logging_provider->stream();
  }

  bool McsLog::IsEnabled() const { return is_enabled_; }

  bool McsLog::IsFatal() const { return is_fatal_; }

  std::ostream &McsLog::ExposeStream() { return *expose_osstream_; }

  McsLog::~McsLog() {
    if (logging_provider_ != nullptr) {
      delete reinterpret_cast<LoggingProvider *>(logging_provider_);
      logging_provider_ = nullptr;
    }
    if (expose_osstream_ != nullptr) {
    }
    if (severity_ == McsLogLevel::FATAL) {
      std::_Exit(EXIT_FAILURE);
    }
  }

} //namespace mcs