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


function(add_jlink_flash_target EXECUTABLE)
    set(TARGET_DEVICE "${DEVICE}")
    get_filename_component(EXEC_NAME ${EXECUTABLE} NAME_WE)
    set(TARGET_EXECUTABLE_HEX "${EXEC_NAME}.hex")
    set(TEMPLATE_FLASH_JLINK_PATH ${CMAKE_SOURCE_DIR}/utilities/flash_script.jlink.in)
    set(JLINK_COMMANDER_EXEC "JLinkExe")
    find_program(JLINK_COMMANDER_EXEC_PATH ${JLINK_COMMANDER_EXEC})
    if(JLINK_COMMANDER_EXEC_PATH)
        if(NOT EXISTS ${TEMPLATE_FLASH_JLINK_PATH})
            message(STATUS "JLink flash template doesn't exist. Flash target will not be added")
        else()
            configure_file(${TEMPLATE_FLASH_JLINK_PATH} ${CMAKE_CURRENT_BINARY_DIR}/flash_script_final.jlink @ONLY)
            add_custom_target(flash DEPENDS build-hex COMMAND ${JLINK_COMMANDER_EXEC_PATH} -CommandFile ${CMAKE_CURRENT_BINARY_DIR}/flash_script_final.jlink)
        endif()
    else()
        message(STATUS "Cannot find JLinkExe (commander). Flash target will not be added")
    endif()
endfunction()


function(add_jlink_erase_target)
    set(TARGET_DEVICE "${DEVICE}")
    set(TEMPLATE_FLASH_JLINK_PATH ${CMAKE_SOURCE_DIR}/utilities/flash_erase_script.jlink.in)
    set(JLINK_COMMANDER_EXEC "JLinkExe")
    find_program(JLINK_COMMANDER_EXEC_PATH ${JLINK_COMMANDER_EXEC})
    if(JLINK_COMMANDER_EXEC_PATH)
        if(NOT EXISTS ${TEMPLATE_FLASH_JLINK_PATH})
            message(STATUS "JLink flash erase template doesn't exist. Erase target will not be added")
        else()
            configure_file(${TEMPLATE_FLASH_JLINK_PATH} ${CMAKE_CURRENT_BINARY_DIR}/flash_erase_script_final.jlink @ONLY)
            add_custom_target(erase COMMAND ${JLINK_COMMANDER_EXEC_PATH} -CommandFile ${CMAKE_CURRENT_BINARY_DIR}/flash_erase_script_final.jlink)
        endif()
    else()
        message(STATUS "Cannot find JLinkExe (commander). Flash target will not be added")
    endif()
endfunction()

