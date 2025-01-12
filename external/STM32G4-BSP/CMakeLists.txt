## Copyright (C) Pablo Rodriguez Nava - All Rights Reserved
##       * Unauthorized copying of this file, via any medium is strictly prohibited
##       * Proprietary and confidential
## Written by Pablo Rodriguez Nava <info@pablintino.com>, October 2021

add_library(
        stm32g4-bsp
        bsp_adc.c
        bsp_can.c
        bsp_clocks.c
        bsp_common_utils.c
        bsp_dma.c
        bsp_i2c.c
        bsp_io.c
        bsp_irq_manager.c
        bsp_tick.c
        bsp_usart.c
        includes/bsp_os.h
)

target_link_libraries(stm32g4-bsp cmsis)
if (ENABLE_UCOS_USAGE)
    target_link_libraries(stm32g4-bsp ucos3-kernel)
    target_compile_definitions(stm32g4-bsp PUBLIC BSP_USING_OS_UCOS)
elseif (ENABLE_FREERTOS_USAGE)
    target_link_libraries(stm32g4-bsp freertos_kernel)
    target_compile_definitions(stm32g4-bsp PUBLIC BSP_USING_OS_FREERTOS)
elseif (ENABLE_THREADX_USAGE)
    target_link_libraries(stm32g4-bsp threadx)
    target_compile_definitions(stm32g4-bsp PUBLIC BSP_USING_OS_THREADX)
else ()
    target_compile_definitions(stm32g4-bsp PUBLIC BSP_NO_OS)
endif ()

target_include_directories(
        stm32g4-bsp
        PUBLIC
        ${CMAKE_CURRENT_LIST_DIR}/includes
)
