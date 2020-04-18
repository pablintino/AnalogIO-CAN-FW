function(add_binary_build_targets TARGET)
    if(RUNTIME_OUTPUT_DIRECTORY)
      set(FILENAME "${RUNTIME_OUTPUT_DIRECTORY}/${TARGET}")
    else()
      set(FILENAME "${TARGET}")
    endif()
    add_custom_target(build-hex DEPENDS ${TARGET} COMMAND ${CMAKE_OBJCOPY} -Oihex ${FILENAME} ${FILENAME}.hex)
    add_custom_target(build-bin DEPENDS ${TARGET} COMMAND ${CMAKE_OBJCOPY} -Obinary ${FILENAME} ${FILENAME}.bin)
endfunction()

function(add_target_size_print_targets TARGET)
    if(RUNTIME_OUTPUT_DIRECTORY)
        set(FILENAME "${RUNTIME_OUTPUT_DIRECTORY}/${TARGET}")
    else()
        set(FILENAME "${TARGET}")
    endif()
    add_custom_command(TARGET ${TARGET} POST_BUILD COMMAND ${CMAKE_SIZE_UTIL} ${FILENAME})
endfunction()