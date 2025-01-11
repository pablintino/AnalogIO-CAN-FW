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


# Add git functions
include(git-functions)

# RTOS Enable/disabled vars as cache variables
set(ENABLE_FREERTOS_USAGE FALSE CACHE BOOL "Use FreeRTOS kernel")
set(ENABLE_UCOS_USAGE FALSE CACHE BOOL "Use uC/OS-III kernel")

# Add freertos as external lib if enabled
if(ENABLE_FREERTOS_USAGE)
    add_subdirectory(FreeRTOS)
endif()

# Add uC/OS-III as external lib if enabled
if(ENABLE_UCOS_USAGE)
    add_subdirectory(uCOS-III)
endif()

# Add ThreadX as external lib if enabled
if(ENABLE_THREADX_USAGE)
    add_subdirectory(threadx)
endif()

add_subdirectory(SEGGER-RTT)

# Add STM32 and CMSIS libs
add_subdirectory(STM32Cube)
add_subdirectory(STM32G4-BSP)