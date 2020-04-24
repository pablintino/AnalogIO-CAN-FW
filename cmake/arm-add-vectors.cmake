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


file(GLOB_RECURSE ASM_STARTUP_FILES "${PROJECT_SOURCE_DIR}/startup/*.s")
list(LENGTH ASM_STARTUP_FILES ASM_STARTUP_FILES_SIZE)

if(NOT DEFINED ARM_STARTUP_FILE AND ASM_STARTUP_FILES_SIZE EQUAL 1)
    list(GET ASM_STARTUP_FILES 0 ARM_STARTUP_FILE)
    message(STATUS "ARM Startup file not defined but defaulted to ${ARM_STARTUP_FILE}")
elseif(NOT DEFINED ARM_STARTUP_FILE)
    message(FATAL_ERROR "ARM Startup file not not provided as ARM_STARTUP_FILE and startup directory is empty or contains more than one file")
endif()

if(NOT EXISTS ${ARM_STARTUP_FILE})
    message(FATAL_ERROR "ARM startup file cannot be located")
else()
    message(STATUS "ARM Startup file ${ARM_STARTUP_FILE}")
endif()

set(ARM_STARTUP_FILE ${ARM_STARTUP_FILE} CACHE FILEPATH "ARM startup file")