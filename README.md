# Colorlog - Fast, Modern, Color Logging with C++
C++ Header-only Color-coded Logging and Error Library with Cross-Platform support and Asynchronous and Synchronous functionality

![image](https://github.com/bgorlick/colorlog_cpp/assets/5460972/ad2dc642-0646-4cce-9bf5-6ec1456c3ce6)

## Installation

### Prerequisites

- A C++20 compatible compiler (clang++ or g++) for colorlog.hpp
- A C++17 compatible compiler for colorlog_cpp17.hpp
 - The reason for different versions is colorlog.hpp uses concepts, for type safety checks, introduced and more formalized in c++20.
- CMake (optional, for building examples)

### Installation with clang++

To compile and try the color log tests you can:

```sh
Clone the repo: 
git clone git@github.com:bgorlick/colorlog_cpp.git

cd colorlog_cpp

Type: make

That should run the Makefile and build the tests. You can also use cmake if you prefer.

mkdir -p build
cd build && cmake ..
make

After building the tests, you can try them out:
./colorlog_unit_test // This will run most of the tests, but you can try individual tests as well.

```

To compile with clang++:

```sh
clang++ -std=c++20 -o colorlog_unit_test colorlog_unit_test.cpp -lpthread
```

### Installation with g++

To compile with g++:

```sh
g++ -std=c++20 -o colorlog_unit_test colorlog_unit_test.cpp -lpthread
```

## Usage

### Include the Header

```cpp
#include "colorlog.hpp"

using namespace colorlog;
```

### Create a Logger Instance

```cpp
LoggerConfig config;
config.log_level = LogLevel::debug;
config.output_mode = OutputMode::Both;
config.log_file_name = "logfile.log";
Logger logger = LoggerFactory::createLogger(config);
logger.info("This is an info message.");
```

### Log a Debug Message

```cpp
logger.debug("This is a debug message.");
```

### Log a Warning Message with File and Line Info

```cpp
logger.warn(__FILE__, __LINE__, "This is a warning message with file and line info.");
```

### Asynchronous Logging

```cpp
AsyncLogger asyncLogger = LoggerFactory::createAsyncLogger(config);
asyncLogger.log(LogLevel::info, __FILE__, __LINE__, "This is an asynchronous info message.");
```

## Key Components

1. LoggerConfig Struct
   - Defines the configuration for the logger.
   - Members:
     - LogLevel log_level = LogLevel::info: Default log level.
     - OutputMode output_mode = OutputMode::Console: Default output mode.
     - std::string log_file_name: Log file name.
     - LogFormatter formatter: Default formatter function.

2. Logger Class
   - Provides logging functionality with color-coded output.
   - Functions:
     - void set_log_level(LogLevel level): Sets the log level.
     - void set_log_level_color(LogLevel level, const std::string& color): Sets the color for a log level.
     - void set_output_mode(OutputMode mode): Sets the output mode.
     - void set_log_file(const std::string& filename): Sets the log file.
     - void set_formatter(LogFormatter formatter): Sets the log formatter.
     - template<PrintableStringOrIterable T> void info(const T& msg): Logs an info message.
     - template<PrintableStringOrIterable T> void debug(const T& msg): Logs a debug message.
     - template<PrintableStringOrIterable T> void warn(const T& msg): Logs a warning message.
     - template<PrintableStringOrIterable T> void error(const T& msg): Logs an error message.
     - template<PrintableStringOrIterable T> void fatal(const T& msg): Logs a fatal message.
     - template<PrintableStringOrIterable T> void trace(const T& msg): Logs a trace message.
     - template<PrintableStringOrIterable T> void info(const std::string& file, int line, const T& msg): Logs an info message with file and line info.
     - template<PrintableStringOrIterable T> void debug(const std::string& file, int line, const T& msg): Logs a debug message with file and line info.
     - template<PrintableStringOrIterable T> void warn(const std::string& file, int line, const T& msg): Logs a warning message with file and line info.
     - template<PrintableStringOrIterable T> void error(const std::string& file, int line, const T& msg): Logs an error message with file and line info.
     - template<PrintableStringOrIterable T> void fatal(const std::string& file, int line, const T& msg): Logs a fatal message with file and line info.
     - template<PrintableStringOrIterable T> void trace(const std::string& file, int line, const T& msg): Logs a trace message with file and line info.
     - template<PrintableStringOrIterableOrOptional T> void log_optional(LogLevel level, const std::string& file, int line, const T& msg): Logs an optional message.
     - template<PrintableOrException T> void log_exception(LogLevel level, const std::string& file, int line, const T& msg): Logs an exception message.
     - void handle_error(const std::exception& e, const std::string& context): Handles an error with context.
     - void register_error_handler(const std::string& exception_type, std::function<void(const std::exception&)> handler): Registers an error handler.
     - void set_default_error_handler(std::function<void(const std::exception&)> handler): Sets the default error handler.

3. AsyncLogger Class
   - Provides asynchronous logging functionality.
   - Functions:
     - template<PrintableStringOrIterable T> void log(LogLevel level, const std::string& file, int line, const T& msg): Asynchronously logs a message.

4. LoggerFactory Class
   - Provides factory methods to create and manage logger instances.
   - Functions:
     - static Logger createLogger(const LoggerConfig& config = LoggerConfig()): Creates a synchronous logger.
     - static AsyncLogger createAsyncLogger(const LoggerConfig& config = LoggerConfig()): Creates an asynchronous logger.

## Copyright

(c) 2024, Benjamin Gorlick | github.com/bgorlick/colorlog_cpp/

## License

Distributed under the Boost Software License, Version 1.0.
(See accompanying file LICENSE_1_0.txt or copy at https://www.boost.org/LICENSE_1_0.txt)

## Contribution

Contributions are welcome! Please fork the repository and submit a pull request.

