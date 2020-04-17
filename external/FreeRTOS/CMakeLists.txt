find_package(Git QUIET)

# Set the expected FreeRTOS base directory
set(FREERTOS_BASE_DIR "${CMAKE_CURRENT_LIST_DIR}/freertos-kernel")

if(GIT_FOUND AND EXISTS "${FREERTOS_BASE_DIR}/.git")
# Update submodules as needed
    option(GIT_SUBMODULE "Check submodules during build" ON)
    if(GIT_SUBMODULE)
        message(STATUS "FreeRTOS submodule update")
        execute_process(COMMAND ${GIT_EXECUTABLE} submodule update --init --recursive
                        WORKING_DIRECTORY ${FREERTOS_BASE_DIR}
                        RESULT_VARIABLE GIT_SUBMOD_RESULT)
        if(NOT GIT_SUBMOD_RESULT EQUAL "0")
            message(FATAL_ERROR "git submodule update --init failed with ${GIT_SUBMOD_RESULT}, please checkout submodules")
        endif()
    endif()
endif()

if(NOT EXISTS "${FREERTOS_BASE_DIR}/tasks.c")
    message(FATAL_ERROR "The submodules were not downloaded! GIT_SUBMODULE was turned off or failed. Please update submodules and try again.")
endif()


set(FREERTOS_INCLUDE_DIRS
    ${FREERTOS_BASE_DIR}/include
    ${CMAKE_CURRENT_LIST_DIR}/include
)

SET(FREERTOS_SOURCES
    ${FREERTOS_BASE_DIR}/tasks.c
    ${FREERTOS_BASE_DIR}/timers.c
    ${FREERTOS_BASE_DIR}/stream_buffer.c
    ${FREERTOS_BASE_DIR}/queue.c
    ${FREERTOS_BASE_DIR}/list.c
    ${FREERTOS_BASE_DIR}/event_groups.c
    # Removed as not used (legacy tasks?) ${FREERTOS_BASE_DIR}/croutine.c
)

if(NOT DEFINED CPU_TYPE)
    message(FATAL_ERROR "-- Not CPU_TYPE defined. Cannot obtain specific port for FreeRTOS")
elseif(${CPU_TYPE} STREQUAL "m0")
    list(APPEND FREERTOS_INCLUDE_DIRS ${FREERTOS_BASE_DIR}/portable/GCC/ARM_CM0)
elseif(${CPU_TYPE} STREQUAL "m3")
    list(APPEND FREERTOS_INCLUDE_DIRS ${FREERTOS_BASE_DIR}/portable/GCC/ARM_CM3)
elseif((${CPU_TYPE} STREQUAL "m4") OR (${CPU_TYPE} STREQUAL "m7"))
    list(APPEND FREERTOS_INCLUDE_DIRS ${FREERTOS_BASE_DIR}/portable/GCC/ARM_CM4F)
else()
    message(FATAL_ERROR "-- CPU_TYPE not supported FreeRTOS")
endif()

add_library(freertos ${FREERTOS_SOURCES})
message("test ${FREERTOS_INCLUDE_DIRS}")
target_include_directories(freertos PUBLIC ${FREERTOS_INCLUDE_DIRS})