set(CMAKE_BUILD_TYPE "Debug" CACHE STRING "Release or debug compilation")
set_property(
  CACHE CMAKE_BUILD_TYPE
  PROPERTY STRINGS
  "Debug" "Release"
)
set(STARTUP_FILE "" CACHE FILEPATH "Startup file path")
set(LINKER_FILE "" CACHE FILEPATH  "LD Linker file path")
# Optional -D compiler definitions
set(OPTIONAL_DEFINITIONS "" CACHE STRING "Optional compiler definitions")
# Optional toolchain path
set(ARM_TOOLCHAIN_DIR "" CACHE STRING "Optional ARM GCC toolchain root path")


if(NOT LINKER_FILE OR LINKER_FILE STREQUAL "")
  message(FATAL_ERROR "LINKER_FILE variable not defined. Provide a valid linker file path")
elseif(NOT IS_ABSOLUTE ${LINKER_FILE})
    get_filename_component(LINKER_FILE "${LINKER_FILE}" REALPATH BASE_DIR "${CMAKE_SOURCE_DIR}")  
endif()
if(NOT EXISTS "${LINKER_FILE}")
    message(FATAL_ERROR "LINKER_FILE path does not exist")
endif()


if(NOT STARTUP_FILE OR STARTUP_FILE STREQUAL "")
  message(FATAL_ERROR "STARTUP_FILE variable not defined. Provide a valid startup file path")
elseif(NOT IS_ABSOLUTE ${STARTUP_FILE})
  get_filename_component(STARTUP_FILE "${STARTUP_FILE}" REALPATH BASE_DIR "${CMAKE_SOURCE_DIR}")  
endif()
if(NOT EXISTS "${STARTUP_FILE}")
    message(FATAL_ERROR "STARTUP_FILE path does not exist")
endif()


