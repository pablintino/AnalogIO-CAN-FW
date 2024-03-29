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


set(CMAKE_SYSTEM_PROCESSOR ARM)

set(DEVICE "" CACHE STRING "MCU device full name")
message(STATUS "Device: ${DEVICE}")

if("${DEVICE}" STREQUAL "")
    message(FATAL_ERROR "No processor defined")
endif()

# Verify if the MCU is a STM32
if(NOT DEVICE MATCHES "^STM32?")
    message(FATAL_ERROR "The given DEVICE is not a STM32 part. Curremtly only SMT32 parts are supported")
endif()

# Extract STM32 family from MCU part
string(REGEX REPLACE "^[sS][tT][mM]32(([fF][0-47])|([hH]7)|([lL][0-14])|([gG]4)|([tT])|([wW])).+$" "\\1" CPU_STM32_FAMILY ${DEVICE})
string(TOUPPER ${CPU_STM32_FAMILY} CPU_STM32_FAMILY)
set(CPU_STM32_FAMILY "${CPU_STM32_FAMILY}" CACHE STRING "STM32 family")

# Check if the part is a supported one
set(STM32_SUPPORTED_FAMILIES L0 L1 L4 F0 F1 F2 F3 F4 F7 H7 G4 CACHE INTERNAL "STM32 supported families")
list(FIND STM32_SUPPORTED_FAMILIES "${CPU_STM32_FAMILY}" FAMILY_INDEX)
if(FAMILY_INDEX EQUAL -1)
    message(FATAL_ERROR "Invalid/unsupported STM32 family: ${CPU_STM32_FAMILY}")
endif()

string(TOUPPER ${DEVICE} DEVICE_U)
string(TOLOWER ${DEVICE} DEVICE_L)

# Determine device family
string(REGEX MATCH "^(STM32[FLG][0-9])" CPU_FAMILY_U "${DEVICE_U}")
string(TOLOWER ${CPU_FAMILY_U} CPU_FAMILY_L)
message(STATUS "Family: ${CPU_FAMILY_U}")


# Determine short device type
string(REGEX MATCH "^(STM32[FLG][0-9][0-9][0-9])" CPU_TYPE_U "${DEVICE_U}")
string(TOLOWER ${CPU_TYPE_U} CPU_TYPE_L)
message(STATUS "Type: ${CPU_TYPE_U}")


# Set CPU type for compiler
if(${CPU_FAMILY_U} STREQUAL "STM32F0")
    set(CPU_TYPE "m0")
elseif(${CPU_FAMILY_U} STREQUAL "STM32F1")
    set(CPU_TYPE "m3")
elseif(${CPU_FAMILY_U} STREQUAL "STM32F3")
    set(CPU_TYPE "m4")
    set(CPU_FPU_ISA "fpv4-sp-d16")
    # TODO!! Extend to other families
    # Select small devices (needs only on x replace)
    string(REGEX REPLACE "(^[sS][tT][mM]32[fF]3[037][1234]).([68BC]).*$" "\\1x\\2" STM32_SDK_DEVICE_DEFINITION ${DEVICE_U})
    # Take into account that if regex doesn't match input == output
    #Select devices that needs xx replace (bigger devices)
    string(REGEX REPLACE "(^[sS][tT][mM]32[fF]3[3579][8]).[68BCE].*$" "\\1xx" STM32_SDK_DEVICE_DEFINITION ${STM32_SDK_DEVICE_DEFINITION})
elseif(${CPU_FAMILY_U} STREQUAL "STM32F4")
    set(CPU_TYPE "m4")
    set(CPU_FPU_ISA "fpv4-sp-d16")
elseif(${CPU_FAMILY_U} STREQUAL "STM32F7")
    set(CPU_TYPE "m7")
    set(CPU_FPU_ISA "fpv5-sp-d16")
elseif(${CPU_FAMILY_U} STREQUAL "STM32L0")
    set(CPU_TYPE "m0+")
elseif(${CPU_FAMILY_U} STREQUAL "STM32L1")
    set(CPU_TYPE "m3")
elseif(${CPU_FAMILY_U} STREQUAL "STM32G4")
    set(CPU_TYPE "m4")
    set(CPU_FPU_ISA "fpv4-sp-d16")
    # Create STM32G4 CMSIS file definitions
    string(REGEX REPLACE "(^[sS][tT][mM]32[gG]4[34789][134])..*$" "\\1xx" STM32_SDK_DEVICE_DEFINITION ${DEVICE_U})
else()
    message(FATAL_ERROR "Unrecognised device family: ${CPU_FAMILY_U}")
endif()

# If no SDK definition can be obtained from device just use the device as definition
if(${STM32_SDK_DEVICE_DEFINITION} STREQUAL "")
    set(STM32_SDK_DEVICE_DEFINITION ${DEVICE})
else()
    message(STATUS "CMSIS Files definition: ${STM32_SDK_DEVICE_DEFINITION}")
endif()

# Add common GLOBAL CPU definitions (all targets)
list(APPEND CPU_DEFINITIONS -D${CPU_TYPE_U}xx -D${CPU_FAMILY_U} -D${STM32_SDK_DEVICE_DEFINITION})
SET(CPU_DEFINITIONS ${CPU_DEFINITIONS})
