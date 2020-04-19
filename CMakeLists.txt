# Set minimum CMake version
cmake_minimum_required(VERSION 3.13)

# Disable in-source build.
if("${CMAKE_SOURCE_DIR}" STREQUAL "${CMAKE_BINARY_DIR}")
    message(FATAL_ERROR "In-source build is not allowed, please use a separate build folder.")
endif()

# Project definition
project(toolchain-demo C CXX ASM)
# Define here the final executable name for the rest of cmake sources
set(EXECUTABLE_NAME ${CMAKE_PROJECT_NAME}.elf CACHE STRING "Executable file name")

# Set default build type to DEBUG if not type was given
if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE DEBUG)
endif()

# Get target specific information
include(stm32)

# Include ARM GCC compiler vars and definitions #TODO Make this call compiler agnostic
include(arm-gcc-configure)

# Add external libraries
add_subdirectory(./external)

# Get top level executable sources and headers
file(GLOB_RECURSE PROJECT_SOURCES "source/*.c")

# Add ARM CPU startup file
include(arm-add-vectors)

# Just create the top-level executable target. CMAKE_PROJECT_NAME as name
add_executable(${EXECUTABLE_NAME} ${PROJECT_SOURCES} ${ARM_STARTUP_FILE})
target_include_directories(${EXECUTABLE_NAME} PUBLIC ${PROJECT_SOURCE_DIR}/includes)

# Add libs to the executable
target_link_libraries(${EXECUTABLE_NAME} freertos stm32hal-full cmsis)

# Add custom build targets
include(arm-gcc-add-custom-targets)
include(arm-jlink-add-custom-targets)
add_binary_build_targets(${EXECUTABLE_NAME})
add_target_size_print_targets(${EXECUTABLE_NAME})
add_jlink_flash_target(${EXECUTABLE_NAME})