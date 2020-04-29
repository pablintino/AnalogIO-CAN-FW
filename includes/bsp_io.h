//
// Created by pablintino on 28/4/20.
//

#ifndef BSP_IO_H
#define BSP_IO_H

#include "stm32f3xx.h"


enum bsp_port_speed_t {
    BSP_IO_LOW = 0,
    BSP_IO_MEDIUM = 1,
    BSP_IO_HIGH = 3
};
typedef enum bsp_port_speed_t bsp_port_speed_t;

enum bsp_port_pp_pd_t {
    BSP_IO_NO_PP_PD = 0,
    BSP_IO_PP = 1,
    BSP_IO_PD = 2
};
typedef enum bsp_port_pp_pd_t bsp_port_pp_pd_t;


void
BSP_conf_output_pin(GPIO_TypeDef *port, uint16_t pin_number, bsp_port_pp_pd_t pull_up_down, bsp_port_speed_t speed);

void BSP_conf_input_pin(GPIO_TypeDef *port, uint16_t pin_number, bsp_port_pp_pd_t pull_up_down, bsp_port_speed_t speed);

void BSP_write_toggle_pin(GPIO_TypeDef *port, uint8_t pin_number);

void BSP_write_pin(GPIO_TypeDef *port, uint8_t pin_number, uint8_t value);

uint8_t BSP_read_pin(GPIO_TypeDef *port, uint8_t pin_number);


#endif //BSP_IO_H
