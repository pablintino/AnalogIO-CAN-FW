# ARM post build commands
add_custom_command(TARGET ${TARGET} POST_BUILD COMMAND ${CMAKE_OBJCOPY} -O binary ${CMAKE_BINARY_DIR}/${TARGET} ${TARGET}.bin)
add_custom_command(TARGET ${TARGET} POST_BUILD COMMAND ${CMAKE_OBJCOPY} -O ihex ${CMAKE_BINARY_DIR}/${TARGET} ${TARGET}.hex)
add_custom_command(TARGET ${TARGET} POST_BUILD COMMAND ${CMAKE_OBJDUMP} -d -S ${CMAKE_BINARY_DIR}/${TARGET} > ${TARGET}.dmp)
add_custom_command(TARGET ${TARGET} POST_BUILD COMMAND ${CMAKE_SIZE_UTIL} ${CMAKE_BINARY_DIR}/${TARGET})