file(GLOB_RECURSE ASM_STARTUP_FILES "${PROJECT_SOURCE_DIR}/startup/*.s")
list(LENGTH ASM_STARTUP_FILES ASM_STARTUP_FILES_SIZE)

message("test ${ASM_STARTUP_FILES}")

if(NOT DEFINED ARM_STARTUP_FILE AND ASM_STARTUP_FILES_SIZE EQUAL 1)
    list(GET ASM_STARTUP_FILES 0 ARM_STARTUP_FILE)
    message(STATUS "ARM Startup file not defined but defaulted to ${ARM_STARTUP_FILE}")
elseif(NOT DEFINED ARM_STARTUP_FILE)
    message(FATAL_ERROR "ARM Startup file not not provided as ARM_STARTUP_FILE and startup directory is empty or contains more than one file")
endif()

if(NOT EXISTS ${ARM_STARTUP_FILE})
    message(FATAL_ERROR "ARM startup file cannot be located")
endif()

set(ARM_STARTUP_FILE ${ARM_STARTUP_FILE} CACHE FILEPATH "ARM startup file")