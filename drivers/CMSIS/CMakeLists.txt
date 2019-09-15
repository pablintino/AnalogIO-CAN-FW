# CMSIS cmake inclusion file
# Adds CMSIS headers to the build


set(DEVICE_BASE "${CMAKE_CURRENT_LIST_DIR}/Device/ST/${CPU_FAMILY_U}xx")

# Add the CMSIS headers
set(STARTUP_BASE ${DEVICE_BASE}/Source/Templates/gcc)
set(CURRENT_LEN 0)



# Find startup file
set(STARTUP_FILE "" CACHE FILEPATH "Startup file path")
if(EXISTS "${STARTUP_FILE}")
    message("Startup file override: ${STARTUP_FILE}")
elseif(EXISTS ${DEVICE_BASE}/Source/Templates/gcc/startup_${DEVICE_L}.s)
    # Simple solution (direct match)
    set(STARTUP_FILE ${DEVICE_BASE}/Source/Templates/gcc/startup_${DEVICE_L}.s)
endif()

if(EXISTS "${STARTUP_FILE}")
    message("Startup file: ${STARTUP_FILE}")
elseif(DEFINED STARTUP_FILE AND NOT STREQUAL "")
    message(FATAL_ERROR "STARTUP_FILE variable not defined. Provide a valid startup file path")
else()
    message(FATAL_ERROR "Startup file ${STARTUP_FILE} not found")
endif()

# Create device library
add_library(cmsis ${STARTUP_FILE})
target_include_directories(cmsis PUBLIC ${CMAKE_CURRENT_LIST_DIR}/Include ${DEVICE_BASE}/Include)