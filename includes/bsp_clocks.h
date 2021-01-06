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


#ifndef BSP_CLOCKS_H
#define BSP_CLOCKS_H

#include "bsp_config.h"
#include "bsp_types.h"
#include "bsp_common_utils.h"
#include "stm32g4xx.h"
#include "stddef.h"


/* Assumed to use 16MHz for STM32G4 family... */
#define BSP_CLK_HSI_VALUE    16000000UL
#define BSP_CLK_LSE_VALUE    32768U

#define BSP_CLK_HSE_READY_TMT 100UL
#define BSP_CLK_HSI_READY_TMT 2UL
#define BSP_CLK_PLL_READY_TMT 2UL


#define BSP_CLK_CLOCK_TYPE_HSE 0x0001U
#define BSP_CLK_CLOCK_TYPE_HSI 0x0002U
#define BSP_CLK_CLOCK_TYPE_PLL 0x0004U

#define BSP_CLK_CLOCK_STATE_HSE_STATE_DISABLE 0x0000U
#define BSP_CLK_CLOCK_STATE_HSE_STATE_ENABLE 0x0001U
#define BSP_CLK_CLOCK_STATE_HSE_STATE_BYPASS 0x0002U

#define IS_HSE_STATE(VALUE) (((VALUE) == BSP_CLK_CLOCK_STATE_HSE_STATE_ENABLE) || ((VALUE) == BSP_CLK_CLOCK_STATE_HSE_STATE_BYPASS))

#define BSP_CLK_CLOCK_STATE_HSI_STATE_DISABLE 0x0000U
#define BSP_CLK_CLOCK_STATE_HSI_STATE_ENABLE 0x0001U

#define IS_HSI_STATE(VALUE) (((VALUE) == BSP_CLK_CLOCK_STATE_HSI_STATE_DISABLE) || ((VALUE) == BSP_CLK_CLOCK_STATE_HSI_STATE_ENABLE))

#define BSP_CLK_CLOCK_STATE_PLL_STATE_DISABLE 0x0000U
#define BSP_CLK_CLOCK_STATE_PLL_STATE_ENABLE 0x0001U

#define BSP_CLK_CLOCK_PLL_SRC_NONE 0
#define BSP_CLK_CLOCK_PLL_SRC_HSI 0x00000002U
#define BSP_CLK_CLOCK_PLL_SRC_HSE 0x00000003U

#define BSP_CLK_IS_PLLN_VALID(VALUE) ((VALUE >= 8UL) && (VALUE <= 127UL))
#define BSP_CLK_IS_PLLM_VALID(VALUE) ((VALUE >= 1UL) && (VALUE <= 16UL))
#define BSP_CLK_IS_PLLP_VALID(VALUE) ((VALUE >= 2UL) && (VALUE <= 31UL))
#define BSP_CLK_IS_PLLQ_VALID(VALUE) ((VALUE == 2UL) || (VALUE == 4UL) || (VALUE == 6UL) || (VALUE == 8UL))
#define BSP_CLK_IS_PLLR_VALID(VALUE) ((VALUE == 2UL) || (VALUE == 4UL) || (VALUE == 6UL) || (VALUE == 8UL))


#define BSP_CLK_CLOCK_TYPE_SYSCLK 0x0001U
#define BSP_CLK_CLOCK_TYPE_HCLK 0x0002U
#define BSP_CLK_CLOCK_TYPE_PCLK1 0x0004U
#define BSP_CLK_CLOCK_TYPE_PCLK2 0x0008U

#define BSP_CLK_CLOCK_SOURCE_PLL 0x00000003U
#define BSP_CLK_CLOCK_SOURCE_HSE 0x00000002U
#define BSP_CLK_CLOCK_SOURCE_HSI 0x00000001U

#define IS_CLOCK_SOURCE(VALUE) (((VALUE) == BSP_CLK_CLOCK_SOURCE_PLL) || ((VALUE) == BSP_CLK_CLOCK_SOURCE_HSE)|| \
                                ((VALUE) == BSP_CLK_CLOCK_SOURCE_HSI))


#define BSP_CLK_AHB_PRESCALER_1     RCC_CFGR_HPRE_DIV1
#define BSP_CLK_AHB_PRESCALER_2     RCC_CFGR_HPRE_DIV2
#define BSP_CLK_AHB_PRESCALER_4     RCC_CFGR_HPRE_DIV4
#define BSP_CLK_AHB_PRESCALER_8     RCC_CFGR_HPRE_DIV8
#define BSP_CLK_AHB_PRESCALER_16    RCC_CFGR_HPRE_DIV16
#define BSP_CLK_AHB_PRESCALER_64    RCC_CFGR_HPRE_DIV64
#define BSP_CLK_AHB_PRESCALER_128   RCC_CFGR_HPRE_DIV128
#define BSP_CLK_AHB_PRESCALER_256   RCC_CFGR_HPRE_DIV256
#define BSP_CLK_AHB_PRESCALER_512   RCC_CFGR_HPRE_DIV512

#define IS_HPRE_DIVIDER(VALUE) (((VALUE) == BSP_CLK_AHB_PRESCALER_1) || ((VALUE) == BSP_CLK_AHB_PRESCALER_2)|| \
                                ((VALUE) == BSP_CLK_AHB_PRESCALER_4)|| ((VALUE) == BSP_CLK_AHB_PRESCALER_8) || \
                                ((VALUE) == BSP_CLK_AHB_PRESCALER_16)|| ((VALUE) == BSP_CLK_AHB_PRESCALER_64) || \
                                ((VALUE) == BSP_CLK_AHB_PRESCALER_128)|| ((VALUE) == BSP_CLK_AHB_PRESCALER_256) || \
                                ((VALUE) == BSP_CLK_AHB_PRESCALER_512))



enum bsp_clk_apb1_prescaler {
    APB1_PRESCALER_1 = RCC_CFGR_PPRE1_DIV1, /*!< APB1 CLK is SYSCLK */
    APB1_PRESCALER_2 = RCC_CFGR_PPRE1_DIV2, /*!< APB1 CLK is SYSCLK/2 */
    APB1_PRESCALER_4 = RCC_CFGR_PPRE1_DIV4, /*!< APB1 CLK is SYSCLK/4 */
    APB1_PRESCALER_8 = RCC_CFGR_PPRE1_DIV8, /*!< APB1 CLK is SYSCLK/8 */
    APB1_PRESCALER_16 = RCC_CFGR_PPRE1_DIV16 /*!< APB1 CLK is SYSCLK/16 */
};

enum bsp_clk_apb2_prescaler {
    APB2_PRESCALER_1 = RCC_CFGR_PPRE2_DIV1, /*!< APB1 CLK is SYSCLK */
    APB2_PRESCALER_2 = RCC_CFGR_PPRE2_DIV2, /*!< APB1 CLK is SYSCLK/2 */
    APB2_PRESCALER_4 = RCC_CFGR_PPRE2_DIV4, /*!< APB1 CLK is SYSCLK/4 */
    APB2_PRESCALER_8 = RCC_CFGR_PPRE2_DIV8, /*!< APB1 CLK is SYSCLK/8 */
    APB2_PRESCALER_16 = RCC_CFGR_PPRE2_DIV16 /*!< APB1 CLK is SYSCLK/16 */
};

enum bsp_clk_enable_clock {
    ENGPIOG = __BSP_BIT_ADDR_OFF_32(offsetof(RCC_TypeDef, AHB2ENR), 6), /*!< GPIO G Port Enable */
    ENGPIOF = __BSP_BIT_ADDR_OFF_32(offsetof(RCC_TypeDef, AHB2ENR), 5), /*!< GPIO F Port Enable */
    ENGPIOE = __BSP_BIT_ADDR_OFF_32(offsetof(RCC_TypeDef, AHB2ENR), 4), /*!< GPIO E Port Enable */
    ENGPIOD = __BSP_BIT_ADDR_OFF_32(offsetof(RCC_TypeDef, AHB2ENR), 3), /*!< GPIO D Port Enable */
    ENGPIOC = __BSP_BIT_ADDR_OFF_32(offsetof(RCC_TypeDef, AHB2ENR), 2), /*!< GPIO C Port Enable */
    ENGPIOB = __BSP_BIT_ADDR_OFF_32(offsetof(RCC_TypeDef, AHB2ENR), 1), /*!< GPIO B Port Enable */
    ENGPIOA = __BSP_BIT_ADDR_OFF_32(offsetof(RCC_TypeDef, AHB2ENR), 0), /*!< GPIO A Port Enable */
    ENUSART1 = __BSP_BIT_ADDR_OFF_32(offsetof(RCC_TypeDef, APB2ENR), 14)  /*!< USART 1 Enable */
};


typedef struct {
    uint32_t PLLState;
    uint32_t PLLSource;
    uint32_t PLLM;
    uint32_t PLLN;
    uint32_t PLLP;
    uint32_t PLLQ;
    uint32_t PLLR;
} bsp_pll_config_t;


typedef struct {
    uint32_t ClockType;
    uint32_t HSEState;
    uint32_t HSIState;
    bsp_pll_config_t PLL;
} bsp_clk_osc_config_t;


typedef struct {
    uint32_t ClockType;
    uint32_t SystemClockSource;
    uint32_t AHBDivider;
    enum bsp_clk_apb1_prescaler APB1_prescaler;
    enum bsp_clk_apb2_prescaler APB2_prescaler;

} bsp_clk_clock_config_t;


#define IS_APB1_DIVIDER(VALUE) (((VALUE) == APB1_PRESCALER_1) || ((VALUE) == APB1_PRESCALER_2)|| \
                                ((VALUE) == APB1_PRESCALER_4)|| ((VALUE) == APB1_PRESCALER_8) || \
                                ((VALUE) == APB1_PRESCALER_16))

#define IS_APB2_DIVIDER(VALUE) (((VALUE) == APB2_PRESCALER_1) || ((VALUE) == APB2_PRESCALER_2)|| \
                                ((VALUE) == APB2_PRESCALER_4)|| ((VALUE) == APB2_PRESCALER_8) || \
                                ((VALUE) == APB2_PRESCALER_16))


ret_status BSP_CLK_config_clocks_osc(const bsp_clk_osc_config_t *oscc);

ret_status BSP_CLK_config_clocks(const bsp_clk_clock_config_t *clkc);

uint32_t BSP_CLK_get_sysclk_freq(void);

uint32_t BSP_CLK_get_hclk_freq(void);

uint32_t BSP_CLK_get_pclk1_freq(void);

uint32_t BSP_CLK_get_pclk2_freq(void);

ret_status BSP_CLK_reset_clocks(void);

void BSP_CLK_enable_periph_clock(enum bsp_clk_enable_clock clock_en);

void BSP_CLK_disable_periph_clock(enum bsp_clk_enable_clock clock_disable);

#endif  //BSP_CLOCKS_H