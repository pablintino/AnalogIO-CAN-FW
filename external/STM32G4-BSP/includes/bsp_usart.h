/* Copyright (C) Pablo Rodriguez Nava - All Rights Reserved
 *       * Unauthorized copying of this file, via any medium is strictly prohibited
 *       * Proprietary and confidential
 * Written by Pablo Rodriguez Nava <info@pablintino.com>, June 2021
 */

#ifndef BSP_USART_H
#define BSP_USART_H

#include "bsp_types.h"
#include "stm32g4xx.h"
#include <stdbool.h>

typedef enum busart_bit_length_e {
    BSP_USART_BIT_LENGTH_7 = USART_CR1_M1,
    BSP_USART_BIT_LENGTH_8 = 0x00000000U,
    BSP_USART_BIT_LENGTH_9 = USART_CR1_M0
} busart_bit_length_t;

typedef enum busart_mode_e {
    BSP_USART_MODE_RX = USART_CR1_RE,
    BSP_USART_MODE_TX = USART_CR1_TE,
    BSP_USART_MODE_RX_TX = USART_CR1_RE | USART_CR1_TE
} busart_mode_t;

typedef enum busart_parity_e {
    BSP_USART_PARITY_NONE = 0x00U,
    BSP_USART_PARITY_EVEN = USART_CR1_PCE,
    BSP_USART_PARITY_ODD = USART_CR1_PCE | USART_CR1_PS
} busart_parity_t;

typedef enum busart_stop_bits_e {
    BSP_USART_STOP_BITS_1 = 0x00U,
    BSP_USART_STOP_BITS_05 = USART_CR2_STOP_0,
    BSP_USART_STOP_BITS_2 = USART_CR2_STOP_1,
    BSP_USART_STOP_BITS_15 = USART_CR2_STOP_0 | USART_CR2_STOP_1
} busart_stop_bits_t;

typedef enum busart_hardware_control_e {
    BSP_USART_HW_CONTROL_NONE = 0x00U,
    BSP_USART_HW_CONTROL_CTS = USART_CR3_CTSE,
    BSP_USART_HW_CONTROL_RTS = USART_CR3_RTSE,
    BSP_USART_HW_CONTROL_CTS_RTS = USART_CR3_CTSE | USART_CR3_RTSE
} busart_hardware_control_t;

typedef enum bsp_usart_clock_source_e {
    BSP_USART_CLK_PCLK = 0x00U,
    BSP_USART_CLK_SYSCLK = 0x01U,
    BSP_USART_CLK_HSI = 0x02U,
    BSP_USART_CLK_LSE = 0x03U
} bsp_usart_clock_source_t;

typedef enum busart_sampling_e {
    BSP_USART_SAMPLING_16_BITS = 0x00U,
    BSP_USART_SAMPLING_8_BITS = USART_CR1_OVER8
} busart_sampling_t;

typedef enum bsp_usart_prescaler_e {
    BSP_USART_PRESCALER_1 = 0x00U,
    BSP_USART_PRESCALER_2 = 0x01U,
    BSP_USART_PRESCALER_4 = 0x02U,
    BSP_USART_PRESCALER_6 = 0x03U,
    BSP_USART_PRESCALER_8 = 0x04U,
    BSP_USART_PRESCALER_10 = 0x05U,
    BSP_USART_PRESCALER_12 = 0x06U,
    BSP_USART_PRESCALER_16 = 0x07U,
    BSP_USART_PRESCALER_32 = 0x08U,
    BSP_USART_PRESCALER_64 = 0x09U,
    BSP_USART_PRESCALER_128 = 0x0AU,
    BSP_USART_PRESCALER_256 = 0x0BU
} busart_prescaler_t;

typedef struct bsp_usart_config_t {
    busart_bit_length_t bit_lengh;
    uint32_t baudrate;
    busart_parity_t parity;
    busart_mode_t Mode;
    busart_stop_bits_t stop_bits;
    busart_hardware_control_t hardware_control;
    busart_sampling_t bit_sampling;
    busart_prescaler_t prescaler;
} bsp_usart_config_t;

typedef USART_TypeDef busart_instance;

ret_status busart_config(busart_instance *usart, const bsp_usart_config_t *config);

ret_status busart_put_char(busart_instance *usart, uint8_t character, uint32_t timeout);

void busart_enable(busart_instance *usart);

void busart_disable(busart_instance *usart);

ret_status busart_config_clk_source(busart_instance *usart, bsp_usart_clock_source_t clock_source);

#endif // BSP_USART_H
