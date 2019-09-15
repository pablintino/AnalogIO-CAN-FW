# Locate sources
file(GLOB BSP_SOURCES ${CMAKE_CURRENT_LIST_DIR}/sources/*.c)

# Create bsp library
add_library(bsp ${BSP_SOURCES} ${BSP_COMPONENT_SOURCES})
target_include_directories(bsp PUBLIC ${CMAKE_CURRENT_LIST_DIR}/includes)
target_link_libraries(bsp hal cmsis)