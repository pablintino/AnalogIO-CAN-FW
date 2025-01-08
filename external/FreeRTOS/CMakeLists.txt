find_package(Git QUIET)

# Set the expected FreeRTOS base directory
set(FREERTOS_BASE_DIR "${CMAKE_CURRENT_LIST_DIR}/freertos-kernel")

if (GIT_FOUND AND EXISTS "${FREERTOS_BASE_DIR}/.git")
    # Update submodules as needed
    option(GIT_SUBMODULE "Check submodules during build" ON)
    if (GIT_SUBMODULE)
        message(STATUS "FreeRTOS submodule update")
        execute_process(COMMAND ${GIT_EXECUTABLE} submodule update --init --recursive
                WORKING_DIRECTORY ${FREERTOS_BASE_DIR}
                RESULT_VARIABLE GIT_SUBMOD_RESULT)
        if (NOT GIT_SUBMOD_RESULT EQUAL "0")
            message(FATAL_ERROR "git submodule update --init failed with ${GIT_SUBMOD_RESULT}, please checkout submodules")
        endif ()
    endif ()
endif ()

if (NOT EXISTS "${FREERTOS_BASE_DIR}/tasks.c")
    message(FATAL_ERROR "The submodules were not downloaded! GIT_SUBMODULE was turned off or failed. Please update submodules and try again.")
else ()
    set(FREERTOS_BASE_DIR "${FREERTOS_BASE_DIR}" CACHE FILEPATH "FreeRTOS base directory")
endif ()


if (NOT DEFINED CPU_TYPE)
    message(FATAL_ERROR "-- Not CPU_TYPE defined. Cannot obtain specific port for FreeRTOS")
elseif (${CPU_TYPE} STREQUAL "m0")
    set(FREERTOS_PORT GCC_ARM_CM0)
elseif (${CPU_TYPE} STREQUAL "m3")
    set(FREERTOS_PORT GCC_ARM_CM3)
elseif ((${CPU_TYPE} STREQUAL "m4") OR (${CPU_TYPE} STREQUAL "m7"))
    set(FREERTOS_PORT GCC_ARM_CM4F)
else ()
    message(FATAL_ERROR "-- CPU_TYPE not supported FreeRTOS")
endif ()

add_library(freertos_config INTERFACE)
target_include_directories(freertos_config SYSTEM INTERFACE ${CMAKE_CURRENT_LIST_DIR}/include)
target_compile_definitions(freertos_config INTERFACE projCOVERAGE_TEST=0)

add_subdirectory(freertos-kernel)
