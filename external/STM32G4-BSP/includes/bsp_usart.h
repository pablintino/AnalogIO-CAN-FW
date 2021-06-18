/* Copyright (C) Pablo Rodriguez Nava - All Rights Reserved
 *       * Unauthorized copying of this file, via any medium is strictly prohibited
 *       * Proprietary and confidential
 * Written by Pablo Rodriguez Nava <info@pablintino.com>, June 2021
 */


#ifndef BSP_USART_H
#define BSP_USART_H


#include <stdbool.h>
#include "bsp_types.h"
#include "stm32g4xx.h"

typedef enum {
    BSP_USART_BIT_LENGTH_7 = USART_CR1_M1,
    BSP_USART_BIT_LENGTH_8 = 0x00000000U,
    BSP_USART_BIT_LENGTH_9 = USART_CR1_M0
} bsp_usart_bit_length;

typedef enum {
    BSP_USART_MODE_RX = USART_CR1_RE,
    BSP_USART_MODE_TX = USART_CR1_TE,
    BSP_USART_MODE_RX_TX = USART_CR1_RE | USART_CR1_TE
} bsp_usart_mode;

typedef enum {
    BSP_USART_PARITY_NONE = 0x00U,
    BSP_USART_PARITY_EVEN = USART_CR1_PCE,
    BSP_USART_PARITY_ODD = USART_CR1_PCE | USART_CR1_PS
} bsp_usart_parity;

typedef enum {
    BSP_USART_STOP_BITS_1 = 0x00U,
    BSP_USART_STOP_BITS_05 = USART_CR2_STOP_0,
    BSP_USART_STOP_BITS_2 = USART_CR2_STOP_1,
    BSP_USART_STOP_BITS_15 = USART_CR2_STOP_0 | USART_CR2_STOP_1
} bsp_usart_stop_bits;

typedef enum {
    BSP_USART_HW_CONTROL_NONE = 0x00U,
    BSP_USART_HW_CONTROL_CTS = USART_CR3_CTSE,
    BSP_USART_HW_CONTROL_RTS = USART_CR3_RTSE,
    BSP_USART_HW_CONTROL_CTS_RTS = USART_CR3_CTSE | USART_CR3_RTSE
} bsp_usart_hardware_control;


enum bsp_usart_clock_source {
    BSP_USART_CLK_PCLK = 0x00U,
    BSP_USART_CLK_SYSCLK = 0x01U,
    BSP_USART_CLK_HSI = 0x02U,
    BSP_USART_CLK_LSE = 0x03U
};

typedef enum {
    BSP_USART_SAMPLING_16_BITS = 0x00U,
    BSP_USART_SAMPLING_8_BITS = USART_CR1_OVER8
} bsp_usart_sampling;

typedef enum {
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
} bsp_usart_prescaler;

typedef struct {
    bsp_usart_bit_length BitLengh;
    uint32_t Baudrate;
    bsp_usart_parity Parity;
    bsp_usart_mode Mode;
    bsp_usart_stop_bits StopBits;
    bsp_usart_hardware_control HardwareControl;
    bsp_usart_sampling BitSampling;
    bsp_usart_prescaler Prescaler;
} bsp_usart_config_t;


typedef USART_TypeDef BSP_USART_Instance;

ret_status BSP_USART_conf(BSP_USART_Instance *usart, bsp_usart_config_t *config);

ret_status BSP_USART_put_char(BSP_USART_Instance *usart, uint8_t character, uint32_t timeout);

void BSP_USART_enable(BSP_USART_Instance *usart);

void BSP_USART_disable(BSP_USART_Instance *usart);

ret_status BSP_USART_conf_clock_source(BSP_USART_Instance *usart, enum bsp_usart_clock_source clock_source);


#endif //BSP_USART_H
