# Add FPU options
if(DEFINED CPU_FPU_ISA AND NOT &{CPU_FPU_ISA} STREQUAL "")
    set(FPU_OPTIONS "-mfpu=${CPU_FPU_ISA} -mfloat-abi=softfp")
else()
    set(FPU_OPTIONS "")
endif()
message("-- FPU Options: ${FPU_OPTIONS}")

# Enable FLTO optimization if required
set(USE_FLTO FALSE CACHE BOOL "Usage of GCC LTO optimization")
if(USE_FLTO)
    set(FLTO_FLAGS "-flto ")
endif()

# Get linker script file
set(LD_LINKER_FILE "" CACHE FILEPATH "Path to GCC LD script")
if(NOT EXISTS "${LD_LINKER_FILE}")
    message(FATAL_ERROR "LD_LINKER_FILE path does not exist")
else()
    message("-- Linker file: ${LD_LINKER_FILE}")
endif()

# Check if this is the first call. If it is just give compiler flags a default value
if( NOT C_FLAGS_INITIALIZED )
    # only do this on the first pass through to avoid overwriting user added options.
    set( C_FLAGS_INITIALIZED "yes" CACHE INTERNAL "Are compiler flags already set?" )
    
    # Set build flags
    set(DEPFLAGS "-MMD -MP")
    set(COMMON_GCC_COMPILE_OPTS "-Wextra -Wall -Wno-unused-parameter -mcpu=cortex-${CPU_TYPE} -mthumb -fno-builtin -ffunction-sections -fdata-sections -fomit-frame-pointer")
  
    set(CMAKE_C_FLAGS "-std=gnu99 ${COMMON_GCC_COMPILE_OPTS} ${FPU_OPTIONS} --specs=nano.specs ${DEPFLAGS}")
    set(CMAKE_CXX_FLAGS "${COMMON_GCC_COMPILE_OPTS} ${FPU_OPTIONS} --specs=nano.specs  ${DEPFLAGS}")
    set(CMAKE_ASM_FLAGS "${COMMON_GCC_COMPILE_OPTS} --specs=nano.specs -x assembler-with-cpp")
    set(CMAKE_EXE_LINKER_FLAGS "${FPU_OPTIONS} -Xlinker -T ${LD_LINKER_FILE} -Wl,-Map=output_linkage.map -Wl,--gc-sections")
  
endif()

# Save the current compiler flags to the cache every time cmake configures the project.
set(CMAKE_C_FLAGS               "${CMAKE_C_FLAGS}"                  CACHE STRING "compiler flags" FORCE)
set(CMAKE_CXX_FLAGS             "${CMAKE_CXX_FLAGS}"                CACHE STRING "compiler flags" FORCE)
set(CMAKE_ASM_FLAGS             "${CMAKE_ASM_FLAGS}"                CACHE STRING "compiler flags" FORCE)
set(CMAKE_EXE_LINKER_FLAGS      "${CMAKE_EXE_LINKER_FLAGS}"         CACHE STRING "linker flags" FORCE)


# Just print the actual value of the flags
message("-- CMAKE_C_FLAGS -> ${CMAKE_C_FLAGS}")
message("-- CMAKE_CXX_FLAGS -> ${CMAKE_CXX_FLAGS}")
message("-- CMAKE_ASM_FLAGS -> ${CMAKE_ASM_FLAGS}")
message("-- CMAKE_EXE_LINKER_FLAGS -> ${CMAKE_EXE_LINKER_FLAGS}")

# Debug Flags
set(COMMON_DEBUG_FLAGS "-O0 -g -gdwarf-2")
set(CMAKE_C_FLAGS_DEBUG   "${COMMON_DEBUG_FLAGS}")
set(CMAKE_CXX_FLAGS_DEBUG "${COMMON_DEBUG_FLAGS}")
set(CMAKE_ASM_FLAGS_DEBUG "${COMMON_DEBUG_FLAGS}")

# Release Flags
set(COMMON_RELEASE_FLAGS "-Os ${FLTO_FLAGS}-DNDEBUG=1 -DRELEASE=1")
set(CMAKE_C_FLAGS_RELEASE 	"${COMMON_RELEASE_FLAGS}")
set(CMAKE_CXX_FLAGS_RELEASE "${COMMON_RELEASE_FLAGS}")
set(CMAKE_ASM_FLAGS_RELEASE "${COMMON_RELEASE_FLAGS}")

# Set default inclusions
link_libraries(-lgcc -lc -lnosys)

