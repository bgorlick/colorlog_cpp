cmake_minimum_required(VERSION 3.10)

project(ColorLog)

# Ensure C++20 is used
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Include directories
include_directories(include)

# Create the library (header-only, so no source files)
add_library(colorlog INTERFACE)
target_include_directories(colorlog INTERFACE include)

# Create test executables for each test file
add_executable(colorlog_async_test tests/colorlog_async_test.cpp)
add_executable(colorlog_concepts_test tests/colorlog_concepts_test.cpp)
add_executable(colorlog_unit_test tests/colorlog_unit_test.cpp)

# Link the test executables with the colorlog library
target_link_libraries(colorlog_async_test colorlog)
target_link_libraries(colorlog_concepts_test colorlog)
target_link_libraries(colorlog_unit_test colorlog)
