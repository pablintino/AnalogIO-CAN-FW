# Append custom modules to module path
list(APPEND CMAKE_MODULE_PATH ${CMAKE_CURRENT_LIST_DIR})


set(CMAKE_SYSTEM_NAME Generic)
set(CMAKE_SYSTEM_PROCESSOR ARM)

# Find GCC for ARM
include(find_compiler)
find_arm_compiler(ARM_COMPILER_CC arm-none-eabi-gcc)
find_arm_compiler(ARM_COMPILER_CXX arm-none-eabi-g++)
find_arm_compiler(ARM_OBJDUMP_BIN arm-none-eabi-objdump)
find_arm_compiler(ARM_OBJCP_BIN arm-none-eabi-objcopy)
find_arm_compiler(ARM_OBJSIZE_BIN arm-none-eabi-size)


# Get GCC binaries path
if(MINGW OR CYGWIN OR WIN32)
    set(UTIL_SEARCH_CMD where)
elseif(UNIX OR APPLE)
    set(UTIL_SEARCH_CMD which)
endif()

execute_process(
  COMMAND ${UTIL_SEARCH_CMD} ${TOOLCHAIN_PREFIX}gcc
  OUTPUT_VARIABLE BINUTILS_PATH
  OUTPUT_STRIP_TRAILING_WHITESPACE
)


# SET tools paths
SET(CMAKE_TRY_COMPILE_TARGET_TYPE STATIC_LIBRARY)
set(CMAKE_C_COMPILER ${ARM_COMPILER_CC} CACHE FILEPATH "C compiler")
set(CMAKE_CXX_COMPILER ${ARM_COMPILER_CXX} CACHE FILEPATH "C++ compiler")
set(CMAKE_ASM_COMPILER ${ARM_COMPILER_CC} CACHE FILEPATH "ASM compiler")
set(CMAKE_OBJCOPY ${ARM_OBJCP_BIN} CACHE FILEPATH "objcopy tool")
set(CMAKE_SIZE_UTIL ${ARM_OBJSIZE_BIN} CACHE FILEPATH "size tool")
set(CMAKE_OBJDUMP ${ARM_OBJDUMP_BIN} CACHE FILEPATH "objdump tool")


set(CMAKE_SYSROOT ${ARM_TOOLCHAIN_PATH}/arm-none-eabi)
set(CMAKE_FIND_ROOT_PATH ${BINUTILS_PATH})
set(CMAKE_FIND_ROOT_PATH_MODE_PROGRAM NEVER)
set(CMAKE_FIND_ROOT_PATH_MODE_LIBRARY ONLY)
set(CMAKE_FIND_ROOT_PATH_MODE_INCLUDE ONLY)
