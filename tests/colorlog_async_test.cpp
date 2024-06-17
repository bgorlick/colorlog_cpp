#include "../include/colorlog.hpp"
#include <iostream>
#include <cassert>
#include <thread>
#include <functional>
#include <stdexcept>
#include <cstdio> // For std::remove

using namespace colorlog;

// Custom formatter for testing
std::string custom_formatter(colorlog::LogLevel, const std::string& file, int line, const std::string& msg) {
    std::ostringstream oss;
    oss << "[" << file << ":" << line << "] " << msg;
    return oss.str();
}

// Custom error handler for testing
void custom_error_handler(const std::exception& e) {
    std::cerr << "Custom handler caught exception: " << e.what() << std::endl;
}

// Function to test synchronous logging with default configuration
void test_sync_logging_default() {
    colorlog::Logger logger;

    logger.info("This is a default info message");
    logger.debug("This is a default debug message");
    logger.warn("This is a default warning message");
    logger.error("This is a default error message");
    logger.fatal("This is a default fatal message");
    logger.trace("This is a default trace message");

    // Test default error handling
    try {
        throw std::runtime_error("Test exception");
    } catch (const std::exception& e) {
        logger.handle_error(e, "test_sync_logging_default");
    }
}

// Function to test synchronous logging with custom configuration
void test_sync_logging_custom() {
    colorlog::LoggerConfig config;
    config.log_level = colorlog::LogLevel::debug;
    config.output_mode = colorlog::OutputMode::Console;
    config.formatter = custom_formatter;

    colorlog::Logger logger(config);

    logger.info("test.cpp", 10, "This is a custom info message");
    logger.debug("test.cpp", 20, "This is a custom debug message");
    logger.warn("test.cpp", 30, "This is a custom warning message");
    logger.error("test.cpp", 40, "This is a custom error message");
    logger.fatal("test.cpp", 50, "This is a custom fatal message");
    logger.trace("test.cpp", 60, "This is a custom trace message");

    // Test custom error handling
    logger.set_default_error_handler(custom_error_handler);
    try {
        throw std::runtime_error("Test exception");
    } catch (const std::exception& e) {
        logger.handle_error(e, "test_sync_logging_custom");
    }
}

// Function to test asynchronous logging
void test_async_logging() {
    colorlog::LoggerConfig config;
    colorlog::AsyncLogger async_logger(config);

    async_logger.log(colorlog::LogLevel::info, "test_async.cpp", 10, "This is an async info message");
    async_logger.log(colorlog::LogLevel::debug, "test_async.cpp", 20, "This is an async debug message");
    async_logger.log(colorlog::LogLevel::warn, "test_async.cpp", 30, "This is an async warning message");
    async_logger.log(colorlog::LogLevel::error, "test_async.cpp", 40, "This is an async error message");
    async_logger.log(colorlog::LogLevel::fatal, "test_async.cpp", 50, "This is an async fatal message");
    async_logger.log(colorlog::LogLevel::trace, "test_async.cpp", 60, "This is an async trace message");

    // Allow some time for async logging to process
    std::this_thread::sleep_for(std::chrono::seconds(1));
}

// Function to test file logging
void test_file_logging() {
    std::string log_file = "test_log.txt";

    // Remove the log file if it exists to ensure a clean start
    std::remove(log_file.c_str());

    colorlog::LoggerConfig config;
    config.log_level = colorlog::LogLevel::debug;
    config.output_mode = colorlog::OutputMode::File;
    config.log_file_name = log_file;
    config.formatter = custom_formatter;

    {
        colorlog::Logger file_logger(config);
        file_logger.info("test_file.cpp", 10, "This is a file info message");
        file_logger.debug("test_file.cpp", 20, "This is a file debug message");
        file_logger.warn("test_file.cpp", 30, "This is a file warning message");
        file_logger.error("test_file.cpp", 40, "This is a file error message");
        file_logger.fatal("test_file.cpp", 50, "This is a file fatal message");
        file_logger.trace("test_file.cpp", 60, "This is a file trace message");
    }

    // Read and validate the log file content
    std::ifstream infile(log_file);
    std::string line;
    int count = 0;
    while (std::getline(infile, line)) {
        std::cout << line << std::endl;
        count++;
    }
    assert(count == 6);  // Ensure all messages were logged
}

int main() {
    std::cout << "Testing synchronous logging with default configuration..." << std::endl;
    test_sync_logging_default();

    std::cout << "Testing synchronous logging with custom configuration..." << std::endl;
    test_sync_logging_custom();

    std::cout << "Testing asynchronous logging..." << std::endl;
    test_async_logging();

    std::cout << "Testing file logging..." << std::endl;
    test_file_logging();

    std::cout << "All tests passed!" << std::endl;
    return 0;
}
