/*
    * colorlog.hpp
    *
    * Header-only library for logging with color-coded output in C++.
    * Supports synchronous and asynchronous logging with customizable log levels and formatters.
    * 
    * Key Components:
    * - LoggerConfig struct: Defines the configuration for the logger.
    * - Logger class: Provides logging functionality with color-coded output.
    * - AsyncLogger class: Provides asynchronous logging functionality.
    * - LoggerFactory class: Provides factory methods to create and manage logger instances.
    * - Templates: Handle different data types for logging messages.
    *
    * External Dependencies:
    * - None
    *
    * (c) 2024, Benjamin Gorlick | github.com/bgorlick/colorlog_cpp/
    * Distributed under the Boost Software License, Version 1.0.
    * (See accompanying file LICENSE_1_0.txt or copy at https://www.boost.org/LICENSE_1_0.txt)
    * 
    * Version 0.0.1
*/

/*
Key Classes and Functions:
1. LoggerConfig Struct
   - Defines the configuration for the logger.
   - Members:
     - `LogLevel log_level = LogLevel::info`: Default log level.
     - `OutputMode output_mode = OutputMode::Console`: Default output mode.
     - `std::string log_file_name`: Log file name.
     - `LogFormatter formatter`: Default formatter function.
2. Logger Class
   - Provides logging functionality with color-coded output.
   - Functions:
     - `void set_log_level(LogLevel level)`: Sets the log level.
     - `void set_log_level_color(LogLevel level, const std::string& color)`: Sets the color for a log level.
     - `void set_output_mode(OutputMode mode)`: Sets the output mode.
     - `void set_log_file(const std::string& filename)`: Sets the log file.
     - `void set_formatter(LogFormatter formatter)`: Sets the log formatter.
     - `template<PrintableStringOrIterable T> void info(const T& msg)`: Logs an info message.
     - `template<PrintableStringOrIterable T> void debug(const T& msg)`: Logs a debug message.
     - `template<PrintableStringOrIterable T> void warn(const T& msg)`: Logs a warning message.
     - `template<PrintableStringOrIterable T> void error(const T& msg)`: Logs an error message.
     - `template<PrintableStringOrIterable T> void fatal(const T& msg)`: Logs a fatal message.
     - `template<PrintableStringOrIterable T> void trace(const T& msg)`: Logs a trace message.
     - `template<PrintableStringOrIterable T> void info(const std::string& file, int line, const T& msg)`: Logs an info message with file and line info.
     - `template<PrintableStringOrIterable T> void debug(const std::string& file, int line, const T& msg)`: Logs a debug message with file and line info.
     - `template<PrintableStringOrIterable T> void warn(const std::string& file, int line, const T& msg)`: Logs a warning message with file and line info.
     - `template<PrintableStringOrIterable T> void error(const std::string& file, int line, const T& msg)`: Logs an error message with file and line info.
     - `template<PrintableStringOrIterable T> void fatal(const std::string& file, int line, const T& msg)`: Logs a fatal message with file and line info.
     - `template<PrintableStringOrIterable T> void trace(const std::string& file, int line, const T& msg)`: Logs a trace message with file and line info.
     - `template<PrintableStringOrIterableOrOptional T> void log_optional(LogLevel level, const std::string& file, int line, const T& msg)`: Logs an optional message.
     - `template<PrintableOrException T> void log_exception(LogLevel level, const std::string& file, int line, const T& msg)`: Logs an exception message.
     - `void handle_error(const std::exception& e, const std::string& context)`: Handles an error with context.
     - `void register_error_handler(const std::string& exception_type, std::function<void(const std::exception&)> handler)`: Registers an error handler.
     - `void set_default_error_handler(std::function<void(const std::exception&)> handler)`: Sets the default error handler.
3. AsyncLogger Class
   - Provides asynchronous logging functionality.
   - Functions:
     - `template<PrintableStringOrIterable T> void log(LogLevel level, const std::string& file, int line, const T& msg)`: Asynchronously logs a message.
4. LoggerFactory Class
   - Provides factory methods to create and manage logger instances.
   - Functions:
     - `static Logger createLogger(const LoggerConfig& config = LoggerConfig())`: Creates a synchronous logger.
     - `static AsyncLogger createAsyncLogger(const LoggerConfig& config = LoggerConfig())`: Creates an asynchronous logger.
*/

/*
------------------
Usage Examples:
------------------

#include "colorlog.hpp"

using namespace colorlog;

*/

/* Example: Create a logger instance 

LoggerConfig config;
config.log_level = LogLevel::debug;
config.output_mode = OutputMode::Both;
config.log_file_name = "logfile.log";
Logger logger = LoggerFactory::createLogger(config);
logger.info("This is an info message.");

*/

/* Expected result: 

[INFO] This is an info message.

*/

/* Example: Log a debug message */
/*

logger.debug("This is a debug message.");

*/

/* Expected result: 

[DEBUG] This is a debug message.

*/

/* Example: Log a warning message with file and line info */
/*

logger.warn(__FILE__, __LINE__, "This is a warning message with file and line info.");

*/

/* Expected result: 

[WARN] file.cpp:line_number This is a warning message with file and line info.

*/

/* Example: Asynchronous logging */
/*

AsyncLogger asyncLogger = LoggerFactory::createAsyncLogger(config);
asyncLogger.log(LogLevel::info, __FILE__, __LINE__, "This is an asynchronous info message.");

*/

/* Expected result: 

[INFO] This is an asynchronous info message.

*/

// File: colorlog.hpp

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
#include <optional>
#include <execinfo.h>
#include <concepts>

// Concepts for type constraints

// Define Arithmetic concept first
/// Concept to check if a type is arithmetic (e.g., int, float).
template <typename T>
concept Arithmetic = std::is_arithmetic_v<T>;

// Helper traits for type checks

/// Trait to check if a type can be printed to an ostream.
template <typename T, typename = void>
struct is_printable : std::false_type {};

template <typename T>
struct is_printable<T, std::void_t<decltype(std::declval<std::ostream&>() << std::declval<T>())>> : std::true_type {};

template <typename T>
inline constexpr bool is_printable_v = is_printable<T>::value;

/// Trait to check if a type is iterable.
template <typename T, typename = void>
struct is_iterable : std::false_type {};

template <typename T>
struct is_iterable<T, std::void_t<decltype(std::declval<T>().begin()), decltype(std::declval<T>().end())>> : std::true_type {};

template <typename T>
inline constexpr bool is_iterable_v = is_iterable<T>::value;

/// Trait to check if a type has a value_type member.
template <typename T, typename = void>
struct has_value_type : std::false_type {};

template <typename T>
struct has_value_type<T, std::void_t<typename T::value_type>> : std::true_type {};

template <typename T>
inline constexpr bool has_value_type_v = has_value_type<T>::value;

/// Trait to check if a type is a string.
template <typename T>
struct is_string : std::is_same<T, std::string> {};

template <typename T>
inline constexpr bool is_string_v = is_string<T>::value;

/// Concept to check if a type can be printed to an ostream.
template <typename T>
concept Printable = is_printable_v<T>;

/// Concept to check if a type is iterable.
template <typename T>
concept Iterable = is_iterable_v<T>;

/// Concept to check if a type is a string or an iterable of printable elements.
template <typename T>
concept StringOrIterableOfPrintable = is_string_v<T> || 
                                      (is_iterable_v<T> && has_value_type_v<T> && Printable<typename T::value_type>);

/// Concept to check if a type is printable, a string, or an iterable of printable elements.
template <typename T>
concept PrintableStringOrIterable = (Printable<T> && !Arithmetic<T>) || StringOrIterableOfPrintable<T>;

/// Concept to check if a type is an optional of printable elements.
template <typename T>
concept PrintableOptional = requires(T t) {
    { t.value() } -> Printable;
};

/// Concept to check if a type is printable, a string, an iterable of printable elements, or an optional of printable elements.
template <typename T>
concept PrintableStringOrIterableOrOptional = PrintableStringOrIterable<T> || PrintableOptional<T>;

/// Concept to check if a type is printable or an exception.
template <typename T>
concept PrintableOrException = (Printable<T> && !Arithmetic<T>) || std::is_base_of_v<std::exception, T>;

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
    template<PrintableStringOrIterable T>
    void info(const T& msg) { log(LogLevel::info, "", 0, msg); }
    
    template<PrintableStringOrIterable T>
    void debug(const T& msg) { log(LogLevel::debug, "", 0, msg); }
    
    template<PrintableStringOrIterable T>
    void warn(const T& msg) { log(LogLevel::warn, "", 0, msg); }
    
    template<PrintableStringOrIterable T>
    void error(const T& msg) { log(LogLevel::error, "", 0, msg); }
    
    template<PrintableStringOrIterable T>
    void fatal(const T& msg) { log(LogLevel::fatal, "", 0, msg); }
    
    template<PrintableStringOrIterable T>
    void trace(const T& msg) { log(LogLevel::trace, "", 0, msg); }

    // Logging functions with file and line information
    template<PrintableStringOrIterable T>
    void info(const std::string& file, int line, const T& msg) { log(LogLevel::info, file, line, msg); }
    
    template<PrintableStringOrIterable T>
    void debug(const std::string& file, int line, const T& msg) { log(LogLevel::debug, file, line, msg); }
    
    template<PrintableStringOrIterable T>
    void warn(const std::string& file, int line, const T& msg) { log(LogLevel::warn, file, line, msg); }
    
    template<PrintableStringOrIterable T>
    void error(const std::string& file, int line, const T& msg) { log(LogLevel::error, file, line, msg); }
    
    template<PrintableStringOrIterable T>
    void fatal(const std::string& file, int line, const T& msg) { log(LogLevel::fatal, file, line, msg); }
    
    template<PrintableStringOrIterable T>
    void trace(const std::string& file, int line, const T& msg) { log(LogLevel::trace, file, line, msg); }

    // Logging functions for PrintableStringOrIterableOrOptional types
    template<PrintableStringOrIterableOrOptional T>
    void log_optional(LogLevel level, const std::string& file, int line, const T& msg) { log(level, file, line, msg); }

    // Logging functions for PrintableOrException types
    template<PrintableOrException T>
    void log_exception(LogLevel level, const std::string& file, int line, const T& msg) { log(level, file, line, msg); }

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
    template<PrintableStringOrIterable T>
    void log(LogLevel level, const std::string& file, int line, const T& msg) {
        if (static_cast<int>(level) < LOG_LEVEL) {
            return;
        }
        try {
            std::ostringstream oss;
            if constexpr (Iterable<T>) {
                for (const auto& item : msg) {
                    oss << item;
                }
            } else {
                oss << msg;
            }
            std::string formattedMsg = formatter_(level, file, line, oss.str());
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
    template<PrintableStringOrIterable T>
    void log(LogLevel level, const std::string& file, int line, const T& msg) {
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
