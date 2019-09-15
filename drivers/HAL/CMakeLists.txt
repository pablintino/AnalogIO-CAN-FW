# HAL cmake inclusion file

# Find hal source files
file(GLOB HAL_SOURCES ${CMAKE_CURRENT_LIST_DIR}/Src/*.c)
file(GLOB HAL_TEMPLATE_SOURCES ${CMAKE_CURRENT_LIST_DIR}/Src/*_template.c)

# Remove _template files from standard sources
foreach(TEMPLATE ${HAL_TEMPLATE_SOURCES})
list(REMOVE_ITEM HAL_SOURCES ${TEMPLATE})
endforeach(TEMPLATE)

# Create hal library
add_library(hal ${HAL_SOURCES})
# Add the hal headers
target_include_directories(hal PUBLIC ${CMAKE_CURRENT_LIST_DIR}/Inc)
target_link_libraries(hal cmsis)