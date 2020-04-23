find_package(Git QUIET)

# Set the expected uC/OS-III base directory
set(UCOS_BASE_DIR "${CMAKE_CURRENT_LIST_DIR}/uCOS-III-kernel")

if(GIT_FOUND AND EXISTS "${UCOS_BASE_DIR}/.git")
# Update submodules as needed
    option(GIT_SUBMODULE "Check submodules during build" ON)
    if(GIT_SUBMODULE)
        message(STATUS "uC/OS-III submodule update")
        execute_process(COMMAND ${GIT_EXECUTABLE} submodule update --init --recursive
                        WORKING_DIRECTORY ${UCOS_BASE_DIR}
                        RESULT_VARIABLE GIT_SUBMOD_RESULT)
        if(NOT GIT_SUBMOD_RESULT EQUAL "0")
            message(FATAL_ERROR "git submodule update --init failed with ${GIT_SUBMOD_RESULT}, please checkout submodules")
        endif()
    endif()
endif()

if(NOT EXISTS "${UCOS_BASE_DIR}/Source/os_core.c")
    message(FATAL_ERROR "The submodules were not downloaded! GIT_SUBMODULE was turned off or failed. Please update submodules and try again.")
else()
    set(UCOS_BASE_DIR "${UCOS_BASE_DIR}" CACHE FILEPATH "uC/OS-III base directory")
endif()


set(UCOS_INCLUDE_DIRS
    ${UCOS_BASE_DIR}/Source
    ${CMAKE_CURRENT_LIST_DIR}/include
)

SET(UCOS_SOURCES
    ${UCOS_BASE_DIR}/Source/os_cfg_app.c
    ${UCOS_BASE_DIR}/Source/os_core.c
    ${UCOS_BASE_DIR}/Source/os_dbg.c
    ${UCOS_BASE_DIR}/Source/os_flag.c
    ${UCOS_BASE_DIR}/Source/os_mem.c
    ${UCOS_BASE_DIR}/Source/os_msg.c
    ${UCOS_BASE_DIR}/Source/os_mutex.c
    ${UCOS_BASE_DIR}/Source/os_prio.c
    ${UCOS_BASE_DIR}/Source/os_q.c
    ${UCOS_BASE_DIR}/Source/os_sem.c
    ${UCOS_BASE_DIR}/Source/os_stat.c
    ${UCOS_BASE_DIR}/Source/os_task.c
    ${UCOS_BASE_DIR}/Source/os_tick.c
    ${UCOS_BASE_DIR}/Source/os_time.c
    ${UCOS_BASE_DIR}/Source/os_tmr.c
    ${UCOS_BASE_DIR}/Source/os_var.c
)

if(NOT DEFINED CPU_TYPE)
    message(FATAL_ERROR "-- Not CPU_TYPE defined. Cannot obtain specific port for uC/OS-III")
elseif(${CPU_TYPE} STREQUAL "m0")
    list(APPEND UCOS_INCLUDE_DIRS ${UCOS_BASE_DIR}/Ports/ARM-Cortex-M/ARMv6-M/GNU)
    list(APPEND UCOS_SOURCES ${UCOS_BASE_DIR}/Ports/ARM-Cortex-M/ARMv6-M/GNU/os_cpu_a.s)
    list(APPEND UCOS_SOURCES ${UCOS_BASE_DIR}/Ports/ARM-Cortex-M/ARMv6-M/os_cpu_c.c)
elseif((${CPU_TYPE} STREQUAL "m3") OR (${CPU_TYPE} STREQUAL "m4") OR (${CPU_TYPE} STREQUAL "m7"))
    list(APPEND UCOS_INCLUDE_DIRS ${UCOS_BASE_DIR}/Ports/ARM-Cortex-M/ARMv7-M/GNU)
    list(APPEND UCOS_SOURCES ${UCOS_BASE_DIR}/Ports/ARM-Cortex-M/ARMv7-M/GNU/os_cpu_a.s)
    list(APPEND UCOS_SOURCES ${UCOS_BASE_DIR}/Ports/ARM-Cortex-M/ARMv7-M/os_cpu_c.c)
else()
    message(FATAL_ERROR "-- CPU_TYPE not supported by uC/OS-III")
endif()

add_library(ucos3-kernel ${UCOS_SOURCES})
target_include_directories(ucos3-kernel PUBLIC ${UCOS_INCLUDE_DIRS})