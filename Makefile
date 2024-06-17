# Makefile for building the ColorLog project

.PHONY: all clean

# Default target
all: build/colorlog_async_test build/colorlog_concepts_test build/colorlog_unit_test
	cp build/colorlog_async_test .
	cp build/colorlog_concepts_test .
	cp build/colorlog_unit_test .

# Build directory
BUILD_DIR := build

# Create the build directory if it doesn't exist
$(BUILD_DIR):
	mkdir -p $(BUILD_DIR)

# Run CMake and build the project
build/colorlog_async_test: $(BUILD_DIR)
	cd $(BUILD_DIR) && cmake .. && $(MAKE)

build/colorlog_concepts_test: $(BUILD_DIR)
	cd $(BUILD_DIR) && cmake .. && $(MAKE)

build/colorlog_unit_test: $(BUILD_DIR)
	cd $(BUILD_DIR) && cmake .. && $(MAKE)

# Clean the build directory
clean:
	rm -rf $(BUILD_DIR)
	rm -f colorlog_async_test colorlog_concepts_test colorlog_unit_test
