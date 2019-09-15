set(CMAKE_BUILD_TYPE "Debug" CACHE STRING "Release or debug compilation")
set_property(
  CACHE CMAKE_BUILD_TYPE
  PROPERTY STRINGS
  "Debug" "Release"
)

set(LINKER_FILE "" CACHE FILEPATH  "LD Linker file path")
if(NOT LINKER_FILE OR LINKER_FILE STREQUAL "")
  message("LINKER_FILE variable not defined. Provide a valid linker file path")
endif()

include(${PROJECT_SOURCE_DIR}/toolchain/stm32.cmake)

# Add optional -D compiler definitions
set(OPTIONAL_DEFINITIONS "" CACHE STRING "Optional compiler definitions")
if(NOT OPTIONAL_DEFINITIONS STREQUAL "")
 add_definitions(${OPTIONAL_DEFINITIONS})
endif()

# Set compiler flags
# Common arguments
add_definitions("-D${DEVICE} -D${CPU_TYPE_U}xx -D${CPU_FAMILY_U} -D${CPU_FAMILY_A} ${OPTIONAL_DEBUG_SYMBOLS}")
set(COMMON_DEFINITIONS "-Wextra -Wall -Wno-unused-parameter -mcpu=cortex-${CPU_TYPE} -mthumb -fno-builtin -ffunction-sections -fdata-sections -fomit-frame-pointer ${OPTIONAL_DEBUG_SYMBOLS}")
set(DEPFLAGS "-MMD -MP")

# Enable FLTO optimization if required
if(USE_FLTO)
	set(OPTFLAGS "-Os -flto")
else()
	set(OPTFLAGS "-Os")
endif()

# Build flags
set(CMAKE_C_FLAGS "-std=gnu99 ${COMMON_DEFINITIONS} --specs=nano.specs ${DEPFLAGS}")
set(CMAKE_CXX_FLAGS "${COMMON_DEFINITIONS} --specs=nano.specs ${DEPFLAGS}")
set(CMAKE_ASM_FLAGS "${COMMON_DEFINITIONS} --specs=nano.specs -x assembler-with-cpp")
set(CMAKE_EXE_LINKER_FLAGS "${COMMON_DEFINITIONS} -Xlinker -T ${LINKER_FILE} -Wl,-Map=${CMAKE_PROJECT_NAME}.map -Wl,--gc-sections")


# Set default inclusions
set(LIKER_LIBS_FLAGS ${LIKER_LIBS_FLAGS} -lgcc -lc -lnosys -lgcc -lc -lnosys)

# Debug Flags
set(COMMON_DEBUG_FLAGS "-O0 -g -gdwarf-2")
set(CMAKE_C_FLAGS_DEBUG   "${COMMON_DEBUG_FLAGS}")
set(CMAKE_CXX_FLAGS_DEBUG "${COMMON_DEBUG_FLAGS}")
set(CMAKE_ASM_FLAGS_DEBUG "${COMMON_DEBUG_FLAGS}")

# Release Flags
set(COMMON_RELEASE_FLAGS "${OPTFLAGS} -DNDEBUG=1 -DRELEASE=1")
set(CMAKE_C_FLAGS_RELEASE 	"${COMMON_RELEASE_FLAGS}")
set(CMAKE_CXX_FLAGS_RELEASE "${COMMON_RELEASE_FLAGS}")
set(CMAKE_ASM_FLAGS_RELEASE "${COMMON_RELEASE_FLAGS}")

