find_package(Git QUIET)

# Set the expected FreeRTOS base directory
set(STM32CUBE_BASE_DIR "${CMAKE_CURRENT_LIST_DIR}/STM32Cube-F3-Src")

if(GIT_FOUND AND EXISTS "${STM32CUBE_BASE_DIR}/.git")
# Update submodules as needed
    option(GIT_SUBMODULE "Check submodules during build" ON)
    if(GIT_SUBMODULE)
        message(STATUS "STM32CubeF3 submodule update")
        execute_process(COMMAND ${GIT_EXECUTABLE} submodule update --init --recursive
                        WORKING_DIRECTORY ${STM32CUBE_BASE_DIR}
                        RESULT_VARIABLE GIT_SUBMOD_RESULT)
        if(NOT GIT_SUBMOD_RESULT EQUAL "0")
            message(FATAL_ERROR "git submodule update --init failed with ${GIT_SUBMOD_RESULT}, please checkout submodules")
        endif()
    endif()
endif()

if(NOT EXISTS "${STM32CUBE_BASE_DIR}/Drivers/CMSIS/Device/ST/STM32F3xx/Include/stm32f3xx.h")
    message(FATAL_ERROR "The submodules were not downloaded! GIT_SUBMODULE was turned off or failed. Please update submodules and try again.")
    else()
    set(FREERTOS_BASE_DIR "${STM32CUBE_BASE_DIR}" CACHE FILEPATH "linker flags")
endif()


# Just set the headers that CMSIS lib will expose
set(CMSIS_INCLUDE_DIRS
    ${STM32CUBE_BASE_DIR}/Drivers/CMSIS/Device/ST/STM32F3xx/Include
    ${STM32CUBE_BASE_DIR}/Drivers/CMSIS/Core/Include
)
# Create the lib as an INTERFACE one
add_library(cmsis INTERFACE)
target_include_directories(cmsis INTERFACE ${CMSIS_INCLUDE_DIRS})


# Search STM32 SDK sources and headers
set(STM32_HAL_FULL_COMPONENTS "" CACHE STRING "STM32 Full SDK components")
if(NOT ${STM32_HAL_FULL_COMPONENTS} STREQUAL "")
    string(REPLACE " " ";" STM32_HAL_FULL_COMPONENTS ${STM32_HAL_FULL_COMPONENTS})
endif()

set(STM32_HAL_FULL_USAGE TRUE CACHE BOOL "Use full STM32 HAL library")
if(NOT STM32_HAL_FULL_USAGE)
    message(STATUS "Skipping full STM32 HAL usage")
else()
    find_package(STM32HAL COMPONENTS ${STM32_HAL_FULL_COMPONENTS} REQUIRED)
    add_library(stm32hal-full ${STM32HAL_SOURCES})
    message(STATUS "Added STM32 full library")
    target_include_directories(stm32hal-full PUBLIC ${STM32HAL_INCLUDE_DIR} ${CMAKE_CURRENT_LIST_DIR}/include)
    target_link_libraries(stm32hal-full cmsis)
endif()

# Low-Level SDK part
set(STM32_LL_HAL_COMPONENTS "" CACHE STRING "STM32 low-level SDK components")
if(NOT ${STM32_LL_HAL_COMPONENTS} STREQUAL "")
    string(REPLACE " " ";" STM32_LL_HAL_COMPONENTS ${STM32_HAL_COMPONENTS})
endif()


set(STM32_HAL_LL_USAGE TRUE CACHE BOOL "Use low-level STM32 HAL library")
if(NOT STM32_HAL_LL_USAGE)
    message(STATUS "Skipping low-level STM32 HAL usage")
else()
    find_package(STM32LL COMPONENTS ${STM32_LL_HAL_COMPONENTS} REQUIRED)
    add_library(stm32hal-ll ${STM32LL_SOURCES})
    message(STATUS "Added STM32 low-level library")
    target_include_directories(stm32hal-ll PUBLIC ${STM32LL_INCLUDE_DIR} ${CMAKE_CURRENT_LIST_DIR}/include)
    target_link_libraries(stm32hal-ll cmsis)
endif()