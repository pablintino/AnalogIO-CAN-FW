/**
 * MIT License
 *
 * Copyright (c) 2020 Pablo Rodriguez Nava, @pablintino
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 *
 **/


#ifndef BSP_IO_H
#define BSP_IO_H

#include "bsp_types.h"
#include "stm32g4xx.h"
#include <stdbool.h>


#define BSP_IO_PORT_LENGTH 16

typedef enum bsp_port_speed_t {
    BSP_IO_LOW  = 0x00,
    BSP_IO_MEDIUM = 0x01,
    BSP_IO_HIGH = 0x02,
    BSP_IO_VERY_HIGH = 0x03
} bsp_port_speed_t;

typedef enum  {
    BSP_IO_NO_PU_PD = 0x00U,
    BSP_IO_PU = 0x01U,
    BSP_IO_PD = 0x02U
} bsp_port_pp_pd_t;

typedef enum  {
    BSP_IO_OUT_TYPE_PP = 0x00U,
    BSP_IO_OUT_TYPE_OPEN_DRAIN = 0x01U
} bsp_port_output_type_t;

typedef enum {
    BSP_IO_PIN_0 = 0x0001U,
    BSP_IO_PIN_1 = 0x0002U,
    BSP_IO_PIN_2 = 0x0004U,
    BSP_IO_PIN_3 = 0x0008U,
    BSP_IO_PIN_4 = 0x0010U,
    BSP_IO_PIN_5 = 0x0020U,
    BSP_IO_PIN_6 = 0x0040U,
    BSP_IO_PIN_7 = 0x0080U,
    BSP_IO_PIN_8 = 0x0100U,
    BSP_IO_PIN_9 = 0x0200U,
    BSP_IO_PIN_10 = 0x0400U,
    BSP_IO_PIN_11 = 0x0800U,
    BSP_IO_PIN_12 = 0x1000U,
    BSP_IO_PIN_13 = 0x2000U,
    BSP_IO_PIN_14 = 0x4000U,
    BSP_IO_PIN_15 = 0x8000U
} bsp_io_pin_number;



#define __BSP_IO_IS_PIN_VALID(PIN) ((PIN >= BSP_IO_PIN_0) && (PIN <= BSP_IO_PIN_15))

typedef GPIO_TypeDef BSP_IO_Port;

#define BSP_IO_IS_PIN_VALID(PIN) (PIN < BSP_IO_PORT_LENGTH)

void
BSP_IO_conf_output_pin(BSP_IO_Port *port, bsp_io_pin_number pin_number, bsp_port_pp_pd_t pull_up_down, bsp_port_speed_t speed, bsp_port_output_type_t output_type);

void BSP_IO_conf_input_pin(BSP_IO_Port *port, bsp_io_pin_number pin_number, bsp_port_pp_pd_t pull_up_down, bsp_port_speed_t speed);

void BSP_IO_conf_af(BSP_IO_Port *port, bsp_io_pin_number pin_number, uint8_t af_function, bsp_port_pp_pd_t pull_up_down, bsp_port_speed_t speed, bsp_port_output_type_t output_type);

void BSP_IO_toggle_pin(BSP_IO_Port *port, uint8_t pin_number);

ret_status BSP_IO_write_pin(BSP_IO_Port *port, uint8_t pin_number, bool value);

uint8_t BSP_IO_read_pin(BSP_IO_Port *port, uint8_t pin_number);


#endif //BSP_IO_H
