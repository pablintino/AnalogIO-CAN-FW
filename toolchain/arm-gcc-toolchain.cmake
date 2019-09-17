set(CMAKE_SYSTEM_NAME Generic)
set(CMAKE_SYSTEM_PROCESSOR ARM)

if(MINGW OR CYGWIN OR WIN32)
    set(EXEC_SUFFIX .exe)
    set(UTIL_SEARCH_CMD where)
elseif(UNIX OR APPLE)
    set(UTIL_SEARCH_CMD which)
endif()

set(TOOLCHAIN_PREFIX arm-none-eabi-)


if("$ENV{ARM_TOOLCHAIN_DIR}" STREQUAL "" AND ${BINUTILS_PATH} NOT STREQUAL "")
    execute_process(
        COMMAND ${UTIL_SEARCH_CMD} ${TOOLCHAIN_PREFIX}gcc
        OUTPUT_VARIABLE BINUTILS_PATH
        OUTPUT_STRIP_TRAILING_WHITESPACE
    )
    get_filename_component(ARM_TOOLCHAIN_DIR ${BINUTILS_PATH} DIRECTORY)
elseif(NOT "$ENV{ARM_TOOLCHAIN_DIR}" STREQUAL "")
    file(TO_CMAKE_PATH $ENV{ARM_TOOLCHAIN_DIR} ARM_TOOLCHAIN_DIR)
endif()

if(NOT DEFINED ARM_TOOLCHAIN_DIR)
    message(FATAL_ERROR "Cannot determine a valid GCC toolchain installation directory")
endif()


# Without that flag CMake is not able to pass test compilation check
if (${CMAKE_VERSION} VERSION_EQUAL "3.6.0" OR ${CMAKE_VERSION} VERSION_GREATER "3.6")
    set(CMAKE_TRY_COMPILE_TARGET_TYPE STATIC_LIBRARY)
else()
    set(CMAKE_EXE_LINKER_FLAGS_INIT "--specs=nosys.specs")
endif()

set(CMAKE_C_COMPILER ${ARM_TOOLCHAIN_DIR}/bin/${TOOLCHAIN_PREFIX}gcc${EXEC_SUFFIX})
set(CMAKE_CXX_COMPILER ${ARM_TOOLCHAIN_DIR}/bin/${TOOLCHAIN_PREFIX}g++${EXEC_SUFFIX})
set(CMAKE_ASM_COMPILER ${CMAKE_C_COMPILER})
set(CMAKE_OBJCOPY ${ARM_TOOLCHAIN_DIR}/bin/${TOOLCHAIN_PREFIX}objcopy${EXEC_SUFFIX} CACHE INTERNAL "objcopy tool")
set(CMAKE_SIZE_UTIL ${ARM_TOOLCHAIN_DIR}/bin/${TOOLCHAIN_PREFIX}size${EXEC_SUFFIX} CACHE INTERNAL "size tool")
set(CMAKE_OBJDUMP ${ARM_TOOLCHAIN_DIR}/bin/${TOOLCHAIN_PREFIX}objdump${EXEC_SUFFIX} CACHE INTERNAL "objdump tool")


set(CMAKE_SYSROOT ${ARM_TOOLCHAIN_DIR}/arm-none-eabi)
set(CMAKE_FIND_ROOT_PATH ${BINUTILS_PATH})
set(CMAKE_FIND_ROOT_PATH_MODE_PROGRAM NEVER)
set(CMAKE_FIND_ROOT_PATH_MODE_LIBRARY ONLY)
set(CMAKE_FIND_ROOT_PATH_MODE_INCLUDE ONLY)