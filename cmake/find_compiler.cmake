# Toolchain file is processed multiple times, however, it cannot access CMake cache on some runs.
# We store the search path in an environment variable so that we can always access it.
if(NOT "${ARM_TOOLCHAIN_PATH}" STREQUAL "")
    set(ENV{ARM_TOOLCHAIN_PATH} "${ARM_TOOLCHAIN_PATH}")
endif()

# Find the compiler executable and store its path in a cache entry ${compiler_path}.
# If not found, issue a fatal message and stop processing. ARM_TOOLCHAIN_PATH can be provided from
# commandline as additional search path.
function(find_arm_compiler compiler_path compiler_exe)
    # Search user provided path first.
    find_program(
        ${compiler_path} ${compiler_exe}
        PATHS $ENV{ARM_TOOLCHAIN_PATH} PATH_SUFFIXES bin
        NO_DEFAULT_PATH
    )
    # If not then search system paths.
    if("${${compiler_path}}" STREQUAL "${compiler_path}-NOTFOUND")
        find_program(${compiler_path} ${compiler_exe})
    endif()
    if("${${compiler_path}}" STREQUAL "${compiler_path}-NOTFOUND")
        set(ARM_TOOLCHAIN_PATH "" CACHE PATH "Path to search for compiler.")
        message(FATAL_ERROR "Compiler not found, you can specify search path with\
            \"ARM_TOOLCHAIN_PATH\".")
    endif()
endfunction()