## MIT License
## 
## Copyright (c) 2020 Pablo Rodriguez Nava, @pablintino
##
## Permission is hereby granted, free of charge, to any person obtaining a copy
## of this software and associated documentation files (the "Software"), to deal
## in the Software without restriction, including without limitation the rights
## to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
## copies of the Software, and to permit persons to whom the Software is
## furnished to do so, subject to the following conditions:
## 
## The above copyright notice and this permission notice shall be included in all
## copies or substantial portions of the Software.
## 
## THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
## IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
## FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
## AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
## LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
## OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
## SOFTWARE.


# Set the expected FreeRTOS base directory
set(STM32CUBE_BASE_DIR "${CMAKE_CURRENT_LIST_DIR}/STM32Cube-Src")

# Update STM32 HAL submodule
# TODO!! Now this submodle is cloned for STM32F3, but the HAL for CPU_STM32_FAMILY should be cloned instead
git_submodule_update(${STM32CUBE_BASE_DIR} "STM32-HAL")

string(TOLOWER ${CPU_STM32_FAMILY} CPU_STM32_FAMILY_L)

if(NOT EXISTS "${STM32CUBE_BASE_DIR}/Drivers/CMSIS/Device/ST/STM32${CPU_STM32_FAMILY}xx/Include/stm32${CPU_STM32_FAMILY_L}xx.h")
    message(FATAL_ERROR "The submodules were not downloaded! GIT_SUBMODULE was turned off or failed. Please update submodules and try again.")
    else()
    set(STM32CUBE_BASE_DIR "${STM32CUBE_BASE_DIR}" CACHE FILEPATH "STM32Cube base directory")
endif()


# Just set the headers that CMSIS lib will expose
set(CMSIS_INCLUDE_DIRS
    ${STM32CUBE_BASE_DIR}/Drivers/CMSIS/Device/ST/STM32${CPU_STM32_FAMILY}xx/Include
    ${STM32CUBE_BASE_DIR}/Drivers/CMSIS/Core/Include
)
# Create the lib as an INTERFACE one
add_library(cmsis INTERFACE)
target_include_directories(cmsis INTERFACE ${CMSIS_INCLUDE_DIRS})


# Search STM32 SDK sources and headers
set(STM32_HAL_FULL_COMPONENTS "" CACHE STRING "STM32 Full SDK components")
if(NOT ${STM32_HAL_FULL_COMPONENTS} STREQUAL "")
    string(REPLACE " " ";" STM32_HAL_FULL_COMPONENTS ${STM32_HAL_FULL_COMPONENTS})
endif()

set(STM32_HAL_FULL_USAGE TRUE CACHE BOOL "Use full STM32 HAL library")
if(NOT STM32_HAL_FULL_USAGE)
    message(STATUS "Skipping full STM32 HAL usage")
else()
    find_package(STM32HAL COMPONENTS ${STM32_HAL_FULL_COMPONENTS} REQUIRED)
    add_library(stm32hal-full ${STM32HAL_SOURCES})
    message(STATUS "Added STM32 full library")
    target_include_directories(stm32hal-full PUBLIC ${STM32HAL_INCLUDE_DIR} ${CMAKE_CURRENT_LIST_DIR}/include)
    target_link_libraries(stm32hal-full cmsis)
endif()

# Low-Level SDK part
set(STM32_LL_HAL_COMPONENTS "" CACHE STRING "STM32 low-level SDK components")
if(NOT ${STM32_LL_HAL_COMPONENTS} STREQUAL "")
    string(REPLACE " " ";" STM32_LL_HAL_COMPONENTS ${STM32_HAL_COMPONENTS})
endif()


set(STM32_HAL_LL_USAGE TRUE CACHE BOOL "Use low-level STM32 HAL library")
if(NOT STM32_HAL_LL_USAGE)
    message(STATUS "Skipping low-level STM32 HAL usage")
else()
    find_package(STM32LL COMPONENTS ${STM32_LL_HAL_COMPONENTS} REQUIRED)
    add_library(stm32hal-ll ${STM32LL_SOURCES})
    message(STATUS "Added STM32 low-level library")
    target_include_directories(stm32hal-ll PUBLIC ${STM32LL_INCLUDE_DIR} ${CMAKE_CURRENT_LIST_DIR}/include)
    target_link_libraries(stm32hal-ll cmsis)
endif()