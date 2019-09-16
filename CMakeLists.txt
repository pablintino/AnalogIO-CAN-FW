###### Project Environment #####

# Set minimum CMake version
cmake_minimum_required(VERSION 2.8)

# Set the compiler (must be prior to project setup)
include(./toolchain/arm-gcc-toolchain.cmake)

set(TARGET "toolchain-demo")

##### Project Setup #####
project(${TARGET} C CXX ASM)

# Set build
if(NOT CMAKE_BUILD_TYPE)
set(CMAKE_BUILD_TYPE DEBUG)
endif()

file(GLOB_RECURSE PROJECT_SOURCES "source/*.c")
include_directories(${PROJECT_SOURCE_DIR}/includes)
add_executable(${TARGET} ${PROJECT_SOURCES})

##### Modules #####
# Generate executable and link

# Include directories
include(${PROJECT_SOURCE_DIR}/toolchain/build.cmake)

# Add base libs (emlib, CMSIS, device files)
add_subdirectory(${PROJECT_SOURCE_DIR}/drivers/BSP)
add_subdirectory(${PROJECT_SOURCE_DIR}/drivers/CMSIS)
add_subdirectory(${PROJECT_SOURCE_DIR}/drivers/HAL)

target_link_libraries(${TARGET} bsp cmsis hal ${LINKER_LIBS_FLAGS})

# Add post build commands
include(${PROJECT_SOURCE_DIR}/toolchain/post-build.cmake)