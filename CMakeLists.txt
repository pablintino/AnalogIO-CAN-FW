
# Set minimum CMake version
cmake_minimum_required(VERSION 3.4)

##### Project Setup #####
project(toolchain-demo C CXX ASM)

# Set default build type to DEBUG if not type was given
if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE DEBUG)
endif()

# Get target specific information
include(stm32)

# Include ARM GCC compiler vars and definitions #TODO Make this call compiler agnostic
include(arm-gcc-configure)

add_subdirectory(./external)


# Get top level executable sources and headers
file(GLOB_RECURSE PROJECT_SOURCES "source/*.c")

# Just create the top-level executable target. CMAKE_PROJECT_NAME as name
add_executable(${CMAKE_PROJECT_NAME} ${PROJECT_SOURCES})
target_include_directories(${CMAKE_PROJECT_NAME} PUBLIC ${PROJECT_SOURCE_DIR}/includes)

# Add libs to the executable
target_link_libraries(${CMAKE_PROJECT_NAME} freertos)
