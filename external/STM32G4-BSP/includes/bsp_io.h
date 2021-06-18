/* Copyright (C) Pablo Rodriguez Nava - All Rights Reserved
 *       * Unauthorized copying of this file, via any medium is strictly prohibited
 *       * Proprietary and confidential
 * Written by Pablo Rodriguez Nava <info@pablintino.com>, June 2021
 */


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
bio_conf_output_port(BSP_IO_Port *port, bsp_io_pin_number pin_number, bsp_port_pp_pd_t pull_up_down, bsp_port_speed_t speed, bsp_port_output_type_t output_type);

void bio_config_input_port(BSP_IO_Port *port, bsp_io_pin_number pin_number, bsp_port_pp_pd_t pull_up_down, bsp_port_speed_t speed);

void bio_config_af_port(BSP_IO_Port *port, bsp_io_pin_number pin_number, uint8_t af_function, bsp_port_pp_pd_t pull_up_down, bsp_port_speed_t speed, bsp_port_output_type_t output_type);

void bio_toggle_port(BSP_IO_Port *port, uint8_t pin_number);

ret_status bio_write_port(BSP_IO_Port *port, uint8_t pin_number, bool value);

uint8_t bio_read_port(BSP_IO_Port *port, uint8_t pin_number);


#endif //BSP_IO_H
