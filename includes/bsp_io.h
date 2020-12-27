//
// Created by pablintino on 28/4/20.
//

#ifndef BSP_IO_H
#define BSP_IO_H

#include "stm32g4xx.h"


typedef enum bsp_port_speed_t {
    BSP_IO_LOW = 0,
    BSP_IO_MEDIUM = 1,
    BSP_IO_HIGH = 3
} bsp_port_speed_t;

typedef enum  {
    BSP_IO_NO_PP_PD = 0,
    BSP_IO_PP = 1,
    BSP_IO_PD = 2
} bsp_port_pp_pd_t;


void
BSP_IO_conf_output_pin(GPIO_TypeDef *port, uint16_t pin_number, bsp_port_pp_pd_t pull_up_down, bsp_port_speed_t speed);

void BSP_IO_conf_input_pin(GPIO_TypeDef *port, uint16_t pin_number, bsp_port_pp_pd_t pull_up_down, bsp_port_speed_t speed);

void BSP_IO_write_toggle_pin(GPIO_TypeDef *port, uint8_t pin_number);

void BSP_IO_write_pin(GPIO_TypeDef *port, uint8_t pin_number, uint8_t value);

uint8_t BSP_IO_read_pin(GPIO_TypeDef *port, uint8_t pin_number);


#endif //BSP_IO_H
