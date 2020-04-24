## MIT License
## 
## Copyright (c) 2020 Pablo Rodriguez Nava, @pablintino
##
## Permission is hereby granted, free of charge, to any person obtaining a copy
## of this software and associated documentation files (the "Software"), to deal
## in the Software without restriction, including without limitation the rights
## to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
## copies of the Software, and to permit persons to whom the Software is
## furnished to do so, subject to the following conditions:
## 
## The above copyright notice and this permission notice shall be included in all
## copies or substantial portions of the Software.
## 
## THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
## IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
## FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
## AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
## LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
## OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
## SOFTWARE.


##################################################################################
##                                                                              ##
##                              uC/OS-LIB Module                                ##
##                                                                              ##
##################################################################################

# Set the expected uC/OS-LIB base directory
set(UCOS_LIB_BASE_DIR "${CMAKE_CURRENT_LIST_DIR}/uCOS-LIB")

# Update LIB submodule
git_submodule_update(${UCOS_LIB_BASE_DIR} "uC/OS-LIB")

# Check if the content is as expected
if(NOT EXISTS "${UCOS_LIB_BASE_DIR}/lib_mem.c")
    message(FATAL_ERROR "The submodules were not downloaded! GIT_SUBMODULE was turned off or failed. Please update submodules and try again.")
else()
    set(UCOS_LIB_BASE_DIR "${UCOS_LIB_BASE_DIR}" CACHE FILEPATH "uC/OS-LIB base directory")
endif()

# Set uC/OS-LIB include paths
set(UCOS_LIB_INCLUDE_DIRS
    ${UCOS_LIB_BASE_DIR}
    ${CMAKE_CURRENT_LIST_DIR}/include
)

# Set uC/OS-LIB sources
SET(UCOS_LIB_SOURCES
    ${UCOS_LIB_BASE_DIR}/lib_ascii.c 
    ${UCOS_LIB_BASE_DIR}/lib_math.c
    ${UCOS_LIB_BASE_DIR}/lib_mem.c
    ${UCOS_LIB_BASE_DIR}/lib_str.c
)

##################################################################################
##                                                                              ##
##                              uC/OS-CPU Module                                ##
##                                                                              ##
##################################################################################

# Set the expected uC/OS-CPU base directory
set(UCOS_CPU_BASE_DIR "${CMAKE_CURRENT_LIST_DIR}/uCOS-CPU")

# Update CPU submodule
git_submodule_update(${UCOS_CPU_BASE_DIR} "uC/OS-CPU")

# Check if the content is as expected
if(NOT EXISTS "${UCOS_CPU_BASE_DIR}/cpu_core.c")
    message(FATAL_ERROR "The submodules were not downloaded! GIT_SUBMODULE was turned off or failed. Please update submodules and try again.")
else()
    set(UCOS_CPU_BASE_DIR "${UCOS_CPU_BASE_DIR}" CACHE FILEPATH "uC/OS-CPU base directory")
endif()

# Set uC/OS-CPU include paths
set(UCOS_CPU_INCLUDE_DIRS
    ${UCOS_CPU_BASE_DIR}
)

# Set uC/OS-CPU sources
SET(UCOS_CPU_SOURCES
    ${UCOS_CPU_BASE_DIR}/cpu_core.c   
)

##################################################################################
##                                                                              ##
##                       uC/OS-CPU and uC/OS-LIB Library                        ##
##                                                                              ##
##################################################################################


if(NOT DEFINED CPU_TYPE)
    message(FATAL_ERROR "-- Not CPU_TYPE defined. Cannot obtain specific port for uC/OS-CPU")
elseif(${CPU_TYPE} STREQUAL "m0")
    list(APPEND UCOS_CPU_INCLUDE_DIRS   ${UCOS_CPU_BASE_DIR}/ARM-Cortex-M/ARMv6-M/GNU)
    list(APPEND UCOS_CPU_SOURCES        ${UCOS_CPU_BASE_DIR}/ARM-Cortex-M/ARMv6-M/GNU/cpu_a.s)
    list(APPEND UCOS_CPU_SOURCES        ${UCOS_CPU_BASE_DIR}/ARM-Cortex-M/ARMv6-M/cpu_c.c)
    list(APPEND UCOS_LIB_SOURCES        ${UCOS_LIB_BASE_DIR}/Ports/ARM/GNU/lib_mem_a.s)
elseif(${CPU_TYPE} STREQUAL "m3")
    list(APPEND UCOS_CPU_INCLUDE_DIRS   ${UCOS_CPU_BASE_DIR}/ARM-Cortex-M/ARMv7-M/GNU)
    list(APPEND UCOS_CPU_SOURCES        ${UCOS_CPU_BASE_DIR}/ARM-Cortex-M/ARMv7-M/GNU/cpu_a.s)
    list(APPEND UCOS_CPU_SOURCES        ${UCOS_CPU_BASE_DIR}/ARM-Cortex-M/ARMv7-M/cpu_c.c)
    list(APPEND UCOS_LIB_SOURCES        ${UCOS_LIB_BASE_DIR}/Ports/ARM-Cortex-M3/GNU/lib_mem_a.s)
elseif((${CPU_TYPE} STREQUAL "m4") OR (${CPU_TYPE} STREQUAL "m7"))
    list(APPEND UCOS_CPU_INCLUDE_DIRS   ${UCOS_CPU_BASE_DIR}/ARM-Cortex-M/ARMv7-M/GNU)
    list(APPEND UCOS_CPU_SOURCES        ${UCOS_CPU_BASE_DIR}/ARM-Cortex-M/ARMv7-M/GNU/cpu_a.s)
    list(APPEND UCOS_CPU_SOURCES        ${UCOS_CPU_BASE_DIR}/ARM-Cortex-M/ARMv7-M/cpu_c.c)
    list(APPEND UCOS_LIB_SOURCES        ${UCOS_LIB_BASE_DIR}/Ports/ARM-Cortex-M4/GNU/lib_mem_a.s)
else()
    message(FATAL_ERROR "-- CPU_TYPE not supported by uC/OS-CPU")
endif()

add_library(ucos3-cpulib ${UCOS_CPU_SOURCES} ${UCOS_LIB_SOURCES})
target_include_directories(ucos3-cpulib PUBLIC ${UCOS_CPU_INCLUDE_DIRS} ${UCOS_LIB_INCLUDE_DIRS} ${CMAKE_CURRENT_LIST_DIR}/include)

##################################################################################
##                                                                              ##
##                        uC/OS-III  Kernel Module                              ##
##                                                                              ##
##################################################################################

# Set the expected uC/OS-III base directory
set(UCOS_KERNEL_BASE_DIR "${CMAKE_CURRENT_LIST_DIR}/uCOS-III-kernel")

# Update kernel submodule
git_submodule_update(${UCOS_KERNEL_BASE_DIR} "uC/OS-III")

# Check if the content is as expected
if(NOT EXISTS "${UCOS_KERNEL_BASE_DIR}/Source/os_core.c")
    message(FATAL_ERROR "The submodules were not downloaded! GIT_SUBMODULE was turned off or failed. Please update submodules and try again.")
else()
    set(UCOS_KERNEL_BASE_DIR "${UCOS_KERNEL_BASE_DIR}" CACHE FILEPATH "uC/OS-III kernel base directory")
endif()

# Set uC/OS-III Kernel include paths
set(UCOS_KERNEL_INCLUDE_DIRS
    ${UCOS_KERNEL_BASE_DIR}/Source
    ${CMAKE_CURRENT_LIST_DIR}/include
)

# Set uC/OS-III Kernel sources
SET(UCOS_KERNEL_SOURCES
    ${UCOS_KERNEL_BASE_DIR}/Source/os_cfg_app.c
    ${UCOS_KERNEL_BASE_DIR}/Source/os_core.c
    ${UCOS_KERNEL_BASE_DIR}/Source/os_dbg.c
    ${UCOS_KERNEL_BASE_DIR}/Source/os_flag.c
    ${UCOS_KERNEL_BASE_DIR}/Source/os_mem.c
    ${UCOS_KERNEL_BASE_DIR}/Source/os_msg.c
    ${UCOS_KERNEL_BASE_DIR}/Source/os_mutex.c
    ${UCOS_KERNEL_BASE_DIR}/Source/os_prio.c
    ${UCOS_KERNEL_BASE_DIR}/Source/os_q.c
    ${UCOS_KERNEL_BASE_DIR}/Source/os_sem.c
    ${UCOS_KERNEL_BASE_DIR}/Source/os_stat.c
    ${UCOS_KERNEL_BASE_DIR}/Source/os_task.c
    ${UCOS_KERNEL_BASE_DIR}/Source/os_tick.c
    ${UCOS_KERNEL_BASE_DIR}/Source/os_time.c
    ${UCOS_KERNEL_BASE_DIR}/Source/os_tmr.c
    ${UCOS_KERNEL_BASE_DIR}/Source/os_var.c
)

if(NOT DEFINED CPU_TYPE)
    message(FATAL_ERROR "-- Not CPU_TYPE defined. Cannot obtain specific port for uC/OS-III")
elseif(${CPU_TYPE} STREQUAL "m0")
    list(APPEND UCOS_KERNEL_INCLUDE_DIRS    ${UCOS_KERNEL_BASE_DIR}/Ports/ARM-Cortex-M/ARMv6-M/GNU)
    list(APPEND UCOS_KERNEL_SOURCES         ${UCOS_KERNEL_BASE_DIR}/Ports/ARM-Cortex-M/ARMv6-M/GNU/os_cpu_a.S)
    list(APPEND UCOS_KERNEL_SOURCES         ${UCOS_KERNEL_BASE_DIR}/Ports/ARM-Cortex-M/ARMv6-M/os_cpu_c.c)
elseif((${CPU_TYPE} STREQUAL "m3") OR (${CPU_TYPE} STREQUAL "m4") OR (${CPU_TYPE} STREQUAL "m7"))
    list(APPEND UCOS_KERNEL_INCLUDE_DIRS    ${UCOS_KERNEL_BASE_DIR}/Ports/ARM-Cortex-M/ARMv7-M/GNU)
    list(APPEND UCOS_KERNEL_SOURCES         ${UCOS_KERNEL_BASE_DIR}/Ports/ARM-Cortex-M/ARMv7-M/GNU/os_cpu_a.S)
    list(APPEND UCOS_KERNEL_SOURCES         ${UCOS_KERNEL_BASE_DIR}/Ports/ARM-Cortex-M/ARMv7-M/os_cpu_c.c)
else()
    message(FATAL_ERROR "-- CPU_TYPE not supported by uC/OS-III")
endif()

add_library(ucos3-kernel ${UCOS_KERNEL_SOURCES})
target_include_directories(ucos3-kernel PUBLIC ${UCOS_KERNEL_INCLUDE_DIRS})
target_link_libraries(ucos3-kernel ucos3-cpulib)