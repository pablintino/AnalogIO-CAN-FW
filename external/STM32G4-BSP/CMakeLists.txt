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
)

## TODO Create some abstraction to remove ucos3
target_link_libraries(stm32g4-bsp cmsis ucos3-kernel)

target_include_directories(
        stm32g4-bsp
        PUBLIC
        ${CMAKE_CURRENT_LIST_DIR}/includes
)
