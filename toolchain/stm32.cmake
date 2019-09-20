# STM32F4 Base CMake file
#
# Configures the project files and environment for any STM32 project

set(DEVICE "" CACHE STRING "STM32 device full name")

if("${DEVICE}" STREQUAL "")
    message(FATAL_ERROR "No processor defined")
endif()

message("Device: ${DEVICE}")
string(TOUPPER ${DEVICE} DEVICE_U)
string(TOLOWER ${DEVICE} DEVICE_L)

# Determine device family
string(REGEX MATCH "^(STM32[FL][0-9])" CPU_FAMILY_U "${DEVICE_U}")
string(TOLOWER ${CPU_FAMILY_U} CPU_FAMILY_L)
message("Family: ${CPU_FAMILY_U}")

# Generic families
string(REGEX MATCH "^(STM32[FL][0-9][0-9][0-9])([A-Z])([A-Z])" CPU_FAMILY_MATCH "${DEVICE_U}")
set(CPU_FAMILY_A "${CMAKE_MATCH_1}x${CMAKE_MATCH_3}")
message("Family Match: ${CPU_FAMILY_A}")

# Determine short device type
string(REGEX MATCH "^(STM32[FL][0-9][0-9][0-9])" CPU_TYPE_U "${DEVICE_U}")
string(TOLOWER ${CPU_TYPE_U} CPU_TYPE_L)
message("Type: ${CPU_TYPE_U}")


# Set CPU type for compiler
if(${CPU_FAMILY_U} STREQUAL "STM32F0")
set(CPU_TYPE "m0")
elseif(${CPU_FAMILY_U} STREQUAL "STM32F1")
set(CPU_TYPE "m3")
elseif(${CPU_FAMILY_U} STREQUAL "STM32F4")
    set(CPU_TYPE "m4")
    set(FPU_OPTIONS "-mfpu=fpv4-sp-d16 -mfloat-abi=softfp")
elseif(${CPU_FAMILY_U} STREQUAL "STM32F7")
    set(CPU_TYPE "m7")
    set(FPU_OPTIONS "-mfpu=fpv5-sp-d16 -mfloat-abi=softfp")
elseif(${CPU_FAMILY_U} STREQUAL "STM32L0")
set(CPU_TYPE "m0+")
elseif(${CPU_FAMILY_U} STREQUAL "STM32L1")
set(CPU_TYPE "m3")
else()
message(FATAL_ERROR "Unrecognised device family: ${CPU_FAMILY_U}")
endif()