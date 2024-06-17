#ifndef COLORLOG_HPP_
#define COLORLOG_HPP_

#include <iostream>
#include <sstream>
#include <string>
#include <unordered_map>
#include <functional>
#include <mutex>
#include <exception>
#include <typeinfo>
#include <fstream>
#include <queue>
#include <thread>
#include <condition_variable>
#include <atomic>
#include <execinfo.h>  

#if defined(_WIN32) || defined(_WIN64)
    #include <io.h>
    #include <windows.h>
    #define ISATTY _isatty
    #define FILENO _fileno
#else
    #include <unistd.h>
    #define ISATTY ::isatty
    #define FILENO ::fileno
#endif

// Define log levels
#define LOG_LEVEL_DEBUG 0
#define LOG_LEVEL_INFO 1
#define LOG_LEVEL_WARN 2
#define LOG_LEVEL_ERROR 3
#define LOG_LEVEL_FATAL 4
#define LOG_LEVEL_TRACE 5

// Default log level
#ifndef LOG_LEVEL
#define LOG_LEVEL LOG_LEVEL_DEBUG
#endif

#if LOG_LEVEL <= LOG_LEVEL_DEBUG
#define LOG_DEBUG(msg) LoggerFactory::instance().log(LogLevel::debug, __FILE__, __LINE__, msg)
#else
#define LOG_DEBUG(msg)
#endif

#if LOG_LEVEL <= LOG_LEVEL_INFO
#define LOG_INFO(msg) LoggerFactory::instance().log(LogLevel::info, __FILE__, __LINE__, msg)
#else
#define LOG_INFO(msg)
#endif

#if LOG_LEVEL <= LOG_LEVEL_WARN
#define LOG_WARN(msg) LoggerFactory::instance().log(LogLevel::warn, __FILE__, __LINE__, msg)
#else
#define LOG_WARN(msg)
#endif

#if LOG_LEVEL <= LOG_LEVEL_ERROR
#define LOG_ERROR(msg) LoggerFactory::instance().log(LogLevel::error, __FILE__, __LINE__, msg)
#else
#define LOG_ERROR(msg)
#endif

#if LOG_LEVEL <= LOG_LEVEL_FATAL
#define LOG_FATAL(msg) LoggerFactory::instance().log(LogLevel::fatal, __FILE__, __LINE__, msg)
#else
#define LOG_FATAL(msg)
#endif

#if LOG_LEVEL <= LOG_LEVEL_TRACE
#define LOG_TRACE(msg) LoggerFactory::instance().log(LogLevel::trace, __FILE__, __LINE__, msg)
#else
#define LOG_TRACE(msg)
#endif

namespace colorlog {

// Global flag to enable/disable colored output
bool is_global_colored = true;

// TerminalCache class for caching terminal information
class TerminalCache {
public:
    // Singleton pattern for single instance
    static TerminalCache& instance() {
        static TerminalCache instance;
        return instance;
    }

    // Check if the given output stream is a terminal
    bool is_terminal(std::ostream* os) {
        if (os == &std::cout) return is_stdout_terminal_;
        if (os == &std::cerr) return is_stderr_terminal_;
        return false;
    }

private:
    // Private constructor for singleton
    TerminalCache() {
        is_stdout_terminal_ = ISATTY(FILENO(stdout));
        is_stderr_terminal_ = ISATTY(FILENO(stderr));
    }

    bool is_stdout_terminal_;  // Cache for stdout terminal status
    bool is_stderr_terminal_;  // Cache for stderr terminal status
};

// Check if color should be applied to the given stream
inline bool check_color(std::ostream& stream) {
    return is_global_colored && TerminalCache::instance().is_terminal(&stream);
}

#if defined(_WIN32) || defined(_WIN64)
// Apply color code for Windows systems
inline void apply_windows_code(std::ostream& stream, WORD attr) {
    if (check_color(stream)) {
        HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
        if (hConsole != INVALID_HANDLE_VALUE) {
            SetConsoleTextAttribute(hConsole, attr);
        }
    }
}
#endif

// Apply color code for non-Windows systems
inline void apply_code(std::ostream& stream, const std::string& code) {
    if (check_color(stream)) {
        stream << code;
    }
}

// Structure to hold color attributes
struct ColorAttr {
    std::string code;  // ANSI color code
#if defined(_WIN32) || defined(_WIN64)
    WORD win_attr;     // Windows color attribute
#endif
};

// Enumeration for log levels
enum class LogLevel { debug, info, warn, error, fatal, trace, unknown };
// Enumeration for output modes
enum class OutputMode { Console, File, Both };

// Predefined color attributes for each log level
struct ColorDefs {
    static const ColorAttr debug;
    static const ColorAttr info;
    static const ColorAttr warn;
    static const ColorAttr error;
    static const ColorAttr fatal;
    static const ColorAttr trace;
    static const ColorAttr unknown;
};

#if defined(_WIN32) || defined(_WIN64)
const ColorAttr ColorDefs::debug = {"\033[36m", FOREGROUND_GREEN | FOREGROUND_BLUE};
const ColorAttr ColorDefs::info = {"\033[1;33m", FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_INTENSITY};
const ColorAttr ColorDefs::warn = {"\033[33m", FOREGROUND_RED | FOREGROUND_GREEN};
const ColorAttr ColorDefs::error = {"\033[1;31m", FOREGROUND_RED | FOREGROUND_INTENSITY};
const ColorAttr ColorDefs::fatal = {"\033[35m", FOREGROUND_RED | FOREGROUND_BLUE};
const ColorAttr ColorDefs::trace = {"\033[32m", FOREGROUND_GREEN};
const ColorAttr ColorDefs::unknown = {"\033[34m", FOREGROUND_BLUE};
#else
const ColorAttr ColorDefs::debug = {"\033[36m"};
const ColorAttr ColorDefs::info = {"\033[1;33m"};
const ColorAttr ColorDefs::warn = {"\033[33m"};
const ColorAttr ColorDefs::error = {"\033[1;31m"};
const ColorAttr ColorDefs::fatal = {"\033[35m"};
const ColorAttr ColorDefs::trace = {"\033[32m"};
const ColorAttr ColorDefs::unknown = {"\033[34m"};
#endif

// Type alias for log formatter function
using LogFormatter = std::function<std::string(LogLevel, const std::string&, int, const std::string&)>;

// Logger configuration structure
struct LoggerConfig {
    LogLevel log_level = LogLevel::info;               // Default log level
    OutputMode output_mode = OutputMode::Console;      // Default output mode
    std::string log_file_name;                         // Log file name
    LogFormatter formatter = [](LogLevel, const std::string& file, int line, const std::string& msg) -> std::string {
        std::ostringstream oss;
        if (!file.empty() && line > 0) {
            oss << file << ":" << line << " ";
        }
        oss << msg;
        return oss.str();
    };  // Default formatter function
};

// Logger class
class Logger {
public:
    // Constructor with optional configuration parameter
    Logger(const LoggerConfig& config = LoggerConfig())
        : log_level_(config.log_level),
          output_mode_(config.output_mode),
          formatter_(config.formatter),
          log_level_colors_{
              {LogLevel::debug, ColorDefs::debug},
              {LogLevel::info, ColorDefs::info},
              {LogLevel::warn, ColorDefs::warn},
              {LogLevel::error, ColorDefs::error},
              {LogLevel::fatal, ColorDefs::fatal},
              {LogLevel::trace, ColorDefs::trace},
              {LogLevel::unknown, ColorDefs::unknown}
          },
          default_handler_([](const std::exception& e) {
              std::cerr << "Unhandled exception: " << e.what() << std::endl;
          }) {
        if (!config.log_file_name.empty()) {
            log_file_.open(config.log_file_name, std::ios::out | std::ios::app);
        }
    }

    Logger(const Logger&) = delete;              // Disable copy constructor
    Logger& operator=(const Logger&) = delete;   // Disable copy assignment

    // Set log level
    void set_log_level(LogLevel level) {
        log_level_ = level;
    }

    // Set log level color
    void set_log_level_color(LogLevel level, const std::string& color) {
        log_level_colors_[level].code = color;
    }

    // Set output mode
    void set_output_mode(OutputMode mode) {
        output_mode_ = mode;
    }

    // Set log file
    void set_log_file(const std::string& filename) {
        std::lock_guard<std::mutex> lock(mutex_);
        log_file_.open(filename, std::ios::out | std::ios::app);
    }

    // Set log formatter
    void set_formatter(LogFormatter formatter) {
        formatter_ = formatter;
    }

    // Logging functions for different log levels
    void info(const std::string& msg) { log(LogLevel::info, "", 0, msg); }
    void debug(const std::string& msg) { log(LogLevel::debug, "", 0, msg); }
    void warn(const std::string& msg) { log(LogLevel::warn, "", 0, msg); }
    void error(const std::string& msg) { log(LogLevel::error, "", 0, msg); }
    void fatal(const std::string& msg) { log(LogLevel::fatal, "", 0, msg); }
    void trace(const std::string& msg) { log(LogLevel::trace, "", 0, msg); }

    // Logging functions with file and line information
    void info(const std::string& file, int line, const std::string& msg) { log(LogLevel::info, file, line, msg); }
    void debug(const std::string& file, int line, const std::string& msg) { log(LogLevel::debug, file, line, msg); }
    void warn(const std::string& file, int line, const std::string& msg) { log(LogLevel::warn, file, line, msg); }
    void error(const std::string& file, int line, const std::string& msg) { log(LogLevel::error, file, line, msg); }
    void fatal(const std::string& file, int line, const std::string& msg) { log(LogLevel::fatal, file, line, msg); }
    void trace(const std::string& file, int line, const std::string& msg) { log(LogLevel::trace, file, line, msg); }

    // Handle exceptions with context
    void handle_error(const std::exception& e, const std::string& context) {
        error(context, 0, e.what());
        log_stack_trace();

        auto it = handlers_.find(typeid(e).name());
        if (it != handlers_.end()) {
            it->second(e);
        } else {
            default_handler_(e);
        }
    }

    // Register error handler for specific exception types
    void register_error_handler(const std::string& exception_type, std::function<void(const std::exception&)> handler) {
        handlers_[exception_type] = handler;
    }

    // Set default error handler
    void set_default_error_handler(std::function<void(const std::exception&)> handler) {
        default_handler_ = handler;
    }

    // Log function with exception safety
    void log(LogLevel level, const std::string& file, int line, const std::string& msg) {
        if (static_cast<int>(level) < LOG_LEVEL) {
            return;
        }
        try {
            std::string formattedMsg = formatter_(level, file, line, msg);
            std::ostream& logStream = getLogStream();

            std::lock_guard<std::mutex> lock(mutex_);

            logStream << "[";
            if (check_color(logStream)) {
                applyLogLevelColor(logStream, level);
                logStream << logLevelToString(level) << "\033[0m";
            } else {
                logStream << logLevelToString(level);
            }
            logStream << "] " << formattedMsg << std::endl;

            if (output_mode_ == OutputMode::Both && log_file_.is_open()) {
                log_file_ << "[" << logLevelToString(level) << "] " << formattedMsg << std::endl;
            }
        } catch (const std::exception& e) {
            std::cerr << "Logging exception: " << e.what() << std::endl;
        }
    }

private:
    // Function to log stack trace
    void log_stack_trace() {
        void *array[10];
        size_t size;
        size = backtrace(array, 10);
        char **messages = backtrace_symbols(array, size);
        std::cerr << "Stack trace:" << std::endl;
        for (size_t i = 0; i < size; i++) {
            std::cerr << messages[i] << std::endl;
        }
        free(messages);
    }

    // Convert log level to string
    std::string logLevelToString(LogLevel level) {
        static std::unordered_map<LogLevel, std::string> defaultMessages = {
            {LogLevel::debug, "DEBUG"},
            {LogLevel::info, "INFO"},
            {LogLevel::warn, "WARNING"},
            {LogLevel::error, "ERROR"},
            {LogLevel::fatal, "FATAL"},
            {LogLevel::trace, "TRACE"},
            {LogLevel::unknown, "UNKNOWN"}
        };
        return defaultMessages[level];
    }

    // Get appropriate log stream based on output mode
    std::ostream& getLogStream() {
        if (output_mode_ == OutputMode::File && log_file_.is_open()) {
            return log_file_;
        } else if (output_mode_ == OutputMode::Both && log_file_.is_open()) {
            return log_file_;
        } else {
            return std::cerr;
        }
    }

    // Apply color to log level based on platform
    std::ostream& applyLogLevelColor(std::ostream& os, LogLevel level) {
#if defined(_WIN32) || defined(_WIN64)
        apply_windows_code(os, log_level_colors_[level].win_attr);
#else
        apply_code(os, log_level_colors_[level].code);
#endif
        return os;
    }

    std::mutex mutex_;  // Mutex for thread safety
    LogLevel log_level_;  // Current log level
    OutputMode output_mode_;  // Current output mode
    LogFormatter formatter_;  // Formatter function
    std::unordered_map<LogLevel, ColorAttr> log_level_colors_;  // Color definitions for log levels
    std::ofstream log_file_;  // Log file stream
    std::unordered_map<std::string, std::function<void(const std::exception&)>> handlers_;  // Exception handlers
    std::function<void(const std::exception&)> default_handler_;  // Default exception handler
};

// Asynchronous logger class
class AsyncLogger {
public:
    // Constructor with optional configuration parameter
    AsyncLogger(const LoggerConfig& config = LoggerConfig())
        : logger_(config), stop_thread_(false) {
        worker_thread_ = std::thread(&AsyncLogger::processQueue, this);
    }

    // Destructor to clean up resources
    ~AsyncLogger() {
        {
            std::lock_guard<std::mutex> lock(queue_mutex_);
            stop_thread_ = true;
        }
        cv_.notify_all();
        worker_thread_.join();
    }

    // Asynchronous log function
    void log(LogLevel level, const std::string& file, int line, const std::string& msg) {
        std::lock_guard<std::mutex> lock(queue_mutex_);
        log_queue_.emplace(level, file, line, msg);
        cv_.notify_one();
    }

private:
    // Process the logging queue
    void processQueue() {
        while (true) {
            std::unique_lock<std::mutex> lock(queue_mutex_);
            cv_.wait(lock, [this] { return !log_queue_.empty() || stop_thread_; });
            while (!log_queue_.empty()) {
                auto& log_entry = log_queue_.front();
                logger_.log(log_entry.level, log_entry.file, log_entry.line, log_entry.msg);
                log_queue_.pop();
            }
            if (stop_thread_) break;
        }
    }

    // Struct to represent a log entry
    struct LogEntry {
        LogLevel level;
        std::string file;
        int line;
        std::string msg;
        LogEntry(LogLevel lvl, const std::string& f, int l, const std::string& m)
            : level(lvl), file(f), line(l), msg(m) {}
    };

    Logger logger_;  // Logger instance
    std::thread worker_thread_;  // Worker thread for processing the queue
    std::queue<LogEntry> log_queue_;  // Queue to store log entries
    std::mutex queue_mutex_;  // Mutex for protecting the queue
    std::condition_variable cv_;  // Condition variable for queue processing
    std::atomic<bool> stop_thread_;  // Flag to stop the worker thread
};

// Factory class for creating logger instances
class LoggerFactory {
public:
    // Create a synchronous logger
    static Logger createLogger(const LoggerConfig& config = LoggerConfig()) {
        return Logger(config);
    }
    // Create an asynchronous logger
    static AsyncLogger createAsyncLogger(const LoggerConfig& config = LoggerConfig()) {
        return AsyncLogger(config);
    }
};

} // namespace colorlog

#endif // COLORLOG_HPP_
