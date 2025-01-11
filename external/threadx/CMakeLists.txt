find_package(Git QUIET)

# Set the expected ThreadX base directory
set(THREADX_BASE_DIR "${CMAKE_CURRENT_LIST_DIR}/threadx-kernel")

if (GIT_FOUND AND EXISTS "${THREADX_BASE_DIR}/.git")
    # Update submodules as needed
    option(GIT_SUBMODULE "Check submodules during build" ON)
    if (GIT_SUBMODULE)
        message(STATUS "ThreadX submodule update")
        execute_process(COMMAND ${GIT_EXECUTABLE} submodule update --init --recursive
                WORKING_DIRECTORY ${THREADX_BASE_DIR}
                RESULT_VARIABLE GIT_SUBMOD_RESULT)
        if (NOT GIT_SUBMOD_RESULT EQUAL "0")
            message(FATAL_ERROR "git submodule update --init failed with ${GIT_SUBMOD_RESULT}, please checkout submodules")
        endif ()
    endif ()
endif ()

if (NOT EXISTS "${THREADX_BASE_DIR}/CMakeLists.txt")
    message(FATAL_ERROR "The submodules were not downloaded! GIT_SUBMODULE was turned off or failed. Please update submodules and try again.")
else ()
    set(THREADX_BASE_DIR "${THREADX_BASE_DIR}" CACHE FILEPATH "ThreadX base directory")
endif ()


set(THREADX_TOOLCHAIN "gnu")
if (NOT DEFINED CPU_TYPE)
    message(FATAL_ERROR "-- Not CPU_TYPE defined. Cannot obtain specific port for ThreadX")
else ()
    set(THREADX_ARCH "cortex_${CPU_TYPE}")
endif ()
if (EXISTS "${CMAKE_CURRENT_LIST_DIR}/include/tx_user.h")
    set(TX_USER_FILE "${CMAKE_CURRENT_LIST_DIR}/include/tx_user.h")
endif ()

add_subdirectory(threadx-kernel)
target_include_directories(threadx
        PUBLIC
        "${CMAKE_CURRENT_LIST_DIR}/include" ,
)
