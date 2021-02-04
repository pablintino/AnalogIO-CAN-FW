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


#include <bsp_tick.h>
#include "bsp_usart.h"
#include "bsp_common_utils.h"
#include "bsp_clocks.h"


const uint16_t USART_PRESCALERS[] = {2, 4, 6, 8, 10, 12, 16, 32, 64, 128, 256};

static uint32_t __calculate_brr(uint32_t usart_clk, uint32_t baudrate, uint16_t prescaler, bsp_usart_sampling sampling);

static ret_status __get_usart_clk_mux_position(BSP_USART_Instance *usart, uint8_t *position);

static ret_status __get_usart_input_frequency(BSP_USART_Instance *usart, uint32_t *freq);

ret_status BSP_USART_conf(BSP_USART_Instance *usart, bsp_usart_config_t *config) {
    uint32_t usart_frequency = 0;
    uint32_t usart_div;
    uint32_t usart_prescaler;
    uint16_t brrtemp;

    /* Most configurations cannot be done if the USART is enabled */
    if (__BSP_IS_FLAG_SET(usart->CR1, USART_CR1_UE)) {
        return STATUS_ERR;
    }
    /* Calculate USART input frequency based on the actual device frequency and the USART/RCC clock mux state */
    if (__get_usart_input_frequency(usart, &usart_frequency) != STATUS_OK) {
        return STATUS_ERR;
    }

    /** Just validate preescaler values before using them */
    if(!__BSP_USART_IS_VALID_PRESCALER(config->Prescaler)){
        return STATUS_ERR;
    }
    usart_prescaler = config->Prescaler != BSP_USART_PRESCALER_1 ? USART_PRESCALERS[config->Prescaler - 1] : 1;
    usart->PRESC = config->Prescaler & USART_PRESC_PRESCALER;
    usart_div = __calculate_brr(usart_frequency, config->Baudrate, usart_prescaler, config->BitSampling);
    if (usart_div < 16 || usart_div > 65535) {
        return STATUS_ERR;
    } else if (config->BitSampling == BSP_USART_SAMPLING_8_BITS) {
        brrtemp = (uint16_t) (usart_div & 0xFFF0U);
        brrtemp |= (uint16_t) ((usart_div & (uint16_t) 0x000FU) >> 1U);
        usart->BRR = brrtemp;
    } else {
        usart->BRR = usart_div;
    }

    __BSP_SET_MASKED_REG_VALUE(usart->CR1,
                               USART_CR1_OVER8 | USART_CR1_RE | USART_CR1_TE | USART_CR1_M | USART_CR1_PCE |
                               USART_CR1_PS,
                               (config->BitSampling & USART_CR1_OVER8) |
                               (config->Parity & (USART_CR1_PCE | USART_CR1_PS)) |
                               (config->Mode & (USART_CR1_RE | USART_CR1_TE)));

    __BSP_SET_MASKED_REG_VALUE(usart->CR2, USART_CR2_STOP, (config->StopBits & USART_CR2_STOP));
    __BSP_SET_MASKED_REG_VALUE(usart->CR3, USART_CR3_CTSE | USART_CR3_RTSE,
                               config->HardwareControl & (USART_CR3_CTSE | USART_CR3_RTSE));

    return STATUS_OK;
}

void BSP_USART_enable(BSP_USART_Instance *usart) {
    __BSP_SET_MASKED_REG(usart->CR1, USART_CR1_UE);
}

void BSP_USART_disable(BSP_USART_Instance *usart) {
    __BSP_CLEAR_MASKED_REG(usart->CR1, USART_CR1_UE);
}


ret_status BSP_USART_conf_clock_source(BSP_USART_Instance *usart, enum bsp_usart_clock_source clock_source) {
    uint8_t position = 0;
    if (__get_usart_clk_mux_position(usart, &position) != STATUS_OK) {
        return STATUS_ERR;
    }
    __BSP_SET_MASKED_REG_VALUE(RCC->CCIPR, 0x03 << position, clock_source << position);
    return STATUS_OK;
}

static ret_status __get_usart_input_frequency(BSP_USART_Instance *usart, uint32_t *freq) {
    uint8_t position = 0;

    if (__get_usart_clk_mux_position(usart, &position) != STATUS_OK) {
        return STATUS_ERR;
    }

    if ((RCC->CCIPR & (0x03U << position)) == (BSP_USART_CLK_HSI << position)) {
        *freq = BSP_CLK_HSI_VALUE;
    } else if ((RCC->CCIPR & (0x03U << position)) == (BSP_USART_CLK_LSE << position)) {
        *freq = BSP_CLK_LSE_VALUE;
    } else if ((RCC->CCIPR & (0x03U << position)) == (BSP_USART_CLK_SYSCLK << position)) {
        *freq = BSP_CLK_get_sysclk_freq();
    } else {
        /* PCLK... Need to check the correct bus for each USART */
        if (usart == USART1) {
            *freq = BSP_CLK_get_pclk2_freq();
        } else {
            *freq = BSP_CLK_get_pclk1_freq();
        }
    }
    return STATUS_OK;
}

static uint32_t
__calculate_brr(uint32_t usart_clk, uint32_t baudrate, uint16_t prescaler, bsp_usart_sampling sampling) {
    return (usart_clk / prescaler * (sampling == BSP_USART_SAMPLING_8_BITS ? 2 : 1) + baudrate / 2) / baudrate;
}

ret_status BSP_USART_put_char(BSP_USART_Instance *usart, uint8_t character, uint32_t timeout) {
    uint32_t tickstart = BSP_TICK_get_ticks();
    ret_status temp_status;
    temp_status = BSP_UTIL_wait_flag_status_now(&usart->ISR, USART_ISR_TXE, USART_ISR_TXE, timeout);
    if (temp_status != STATUS_OK) {
        return temp_status;
    }

    usart->TDR = character;
    return BSP_UTIL_wait_flag_status_now(&usart->ISR, USART_ISR_TC, USART_ISR_TC,
                                         timeout - (BSP_TICK_get_ticks() - tickstart));
}

static ret_status __get_usart_clk_mux_position(BSP_USART_Instance *usart, uint8_t *position) {
    if (usart == USART1) {
        *position = 0;
    } else if (usart == USART2) {
        *position = 2;
    } else if (usart == USART3) {
        *position = 4;
    } else if (usart == UART4) {
        *position = 6;
#if defined(UART5)
        }else if (usart == UART5){
        position = 8;
#endif
    } else {
        return STATUS_ERR;
    }
    return STATUS_OK;
}