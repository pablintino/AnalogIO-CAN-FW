/* Copyright (C) Pablo Rodriguez Nava - All Rights Reserved
 *       * Unauthorized copying of this file, via any medium is strictly prohibited
 *       * Proprietary and confidential
 * Written by Pablo Rodriguez Nava <info@pablintino.com>, June 2021
 */

#include "includes/bsp_usart.h"
#include "includes/bsp_clocks.h"
#include "includes/bsp_common_utils.h"
#include "includes/bsp_tick.h"

const uint16_t USART_PRESCALERS[] = {2, 4, 6, 8, 10, 12, 16, 32, 64, 128, 256};

static uint32_t __calculate_brr(uint32_t usart_clk, uint32_t baudrate, uint16_t prescaler, busart_sampling_t sampling);

static ret_status __get_usart_clk_mux_position(const busart_instance *usart, uint8_t *position);

static ret_status __get_usart_input_frequency(const busart_instance *usart, uint32_t *freq);

static inline bool __is_valid_prescaler(busart_prescaler_t prescaler)
{
    return ((prescaler) == BSP_USART_PRESCALER_1) || ((prescaler) == BSP_USART_PRESCALER_2) ||
           ((prescaler) == BSP_USART_PRESCALER_4) || ((prescaler) == BSP_USART_PRESCALER_6) ||
           ((prescaler) == BSP_USART_PRESCALER_8) || ((prescaler) == BSP_USART_PRESCALER_10) ||
           ((prescaler) == BSP_USART_PRESCALER_12) || ((prescaler) == BSP_USART_PRESCALER_16) ||
           ((prescaler) == BSP_USART_PRESCALER_32) || ((prescaler) == BSP_USART_PRESCALER_64) ||
           ((prescaler) == BSP_USART_PRESCALER_128) || ((prescaler) == BSP_USART_PRESCALER_256);
}

ret_status busart_config(busart_instance *usart, const bsp_usart_config_t *config)
{
    /* Most configurations cannot be done if the USART is enabled */
    if (__BSP_IS_FLAG_SET(usart->CR1, USART_CR1_UE)) {
        return STATUS_ERR;
    }

    uint32_t usart_frequency;
    /* Calculate USART input frequency based on the actual device frequency and the USART/RCC clock mux state */
    if (__get_usart_input_frequency(usart, &usart_frequency) != STATUS_OK) {
        return STATUS_ERR;
    }

    /** Just validate preescaler values before using them */
    if (!__is_valid_prescaler(config->prescaler)) {
        return STATUS_ERR;
    }
    const uint32_t usart_prescaler =
        config->prescaler != BSP_USART_PRESCALER_1 ? USART_PRESCALERS[config->prescaler - 1] : 1;
    usart->PRESC = config->prescaler & USART_PRESC_PRESCALER;
    const uint32_t usart_div =
        __calculate_brr(usart_frequency, config->baudrate, usart_prescaler, config->bit_sampling);
    if (usart_div < 16 || usart_div > 65535) {
        return STATUS_ERR;
    } else if (config->bit_sampling == BSP_USART_SAMPLING_8_BITS) {
        uint16_t brrtemp = (uint16_t)(usart_div & 0xFFF0U);
        brrtemp |= (uint16_t)((usart_div & (uint16_t)0x000FU) >> 1U);
        usart->BRR = brrtemp;
    } else {
        usart->BRR = usart_div;
    }

    __BSP_SET_MASKED_REG_VALUE(
        usart->CR1,
        USART_CR1_OVER8 | USART_CR1_RE | USART_CR1_TE | USART_CR1_M | USART_CR1_PCE | USART_CR1_PS,
        (config->bit_sampling & USART_CR1_OVER8) | (config->parity & (USART_CR1_PCE | USART_CR1_PS)) |
            (config->Mode & (USART_CR1_RE | USART_CR1_TE)));

    __BSP_SET_MASKED_REG_VALUE(usart->CR2, USART_CR2_STOP, (config->stop_bits & USART_CR2_STOP));
    __BSP_SET_MASKED_REG_VALUE(
        usart->CR3, USART_CR3_CTSE | USART_CR3_RTSE, config->hardware_control & (USART_CR3_CTSE | USART_CR3_RTSE));

    return STATUS_OK;
}

void busart_enable(busart_instance *usart)
{
    __BSP_SET_MASKED_REG(usart->CR1, USART_CR1_UE);
}

void busart_disable(busart_instance *usart)
{
    __BSP_CLEAR_MASKED_REG(usart->CR1, USART_CR1_UE);
}

ret_status busart_config_clk_source(busart_instance *usart, bsp_usart_clock_source_t clock_source)
{
    uint8_t position = 0;
    if (__get_usart_clk_mux_position(usart, &position) != STATUS_OK) {
        return STATUS_ERR;
    }
    __BSP_SET_MASKED_REG_VALUE(RCC->CCIPR, 0x03 << position, clock_source << position);
    return STATUS_OK;
}

ret_status busart_put_char(busart_instance *usart, uint8_t character, uint32_t timeout)
{
    ret_status temp_status;
    const uint32_t tickstart = btick_get_ticks();
    temp_status = butil_wait_flag_status_now(&usart->ISR, USART_ISR_TXE, USART_ISR_TXE, timeout);
    if (temp_status != STATUS_OK) {
        return temp_status;
    }

    usart->TDR = character;
    return butil_wait_flag_status_now(
        &usart->ISR, USART_ISR_TC, USART_ISR_TC, timeout - (btick_get_ticks() - tickstart));
}

static ret_status __get_usart_input_frequency(const busart_instance *usart, uint32_t *freq)
{
    uint8_t position = 0;

    if (__get_usart_clk_mux_position(usart, &position) != STATUS_OK) {
        return STATUS_ERR;
    }

    if ((RCC->CCIPR & (0x03U << position)) == (uint32_t)(BSP_USART_CLK_HSI << position)) {
        *freq = BSP_CLK_HSI_VALUE;
    } else if ((RCC->CCIPR & (0x03U << position)) == (uint32_t)(BSP_USART_CLK_LSE << position)) {
        *freq = BSP_CLK_LSE_VALUE;
    } else if ((RCC->CCIPR & (0x03U << position)) == (uint32_t)(BSP_USART_CLK_SYSCLK << position)) {
        *freq = bclk_get_sysclk_freq();
    } else {
        /* PCLK... Need to check the correct bus for each USART */
        if (usart == USART1) {
            *freq = bclk_get_pclk2_freq();
        } else {
            *freq = bclk_get_pclk1_freq();
        }
    }
    return STATUS_OK;
}

static uint32_t __calculate_brr(uint32_t usart_clk, uint32_t baudrate, uint16_t prescaler, busart_sampling_t sampling)
{
    return (usart_clk / prescaler * (sampling == BSP_USART_SAMPLING_8_BITS ? 2 : 1) + baudrate / 2) / baudrate;
}

static ret_status __get_usart_clk_mux_position(const busart_instance *usart, uint8_t *position)
{
    if (usart == USART1) {
        *position = 0;
    } else if (usart == USART2) {
        *position = 2;
    } else if (usart == USART3) {
        *position = 4;
    } else if (usart == UART4) {
        *position = 6;
#if defined(UART5)
    } else if (usart == UART5) {
        *position = 8;
#endif
    } else {
        return STATUS_ERR;
    }
    return STATUS_OK;
}