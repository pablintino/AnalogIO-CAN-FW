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

#include "stm32g4xx.h"
#include "bsp_config.h"
#include "bsp_types.h"

/* Assumed to use 16MHz for STM32G4 family... */
#define BSP_CLK_HSI_VALUE    16000000UL

#define BSP_CLK_HSE_READY_TMT 100UL
#define BSP_CLK_HSI_READY_TMT 2UL
#define BSP_CLK_PLL_READY_TMT 2UL



#define BSP_CLK_CLOCK_TYPE_HSE 0x0001U
#define BSP_CLK_CLOCK_TYPE_HSI 0x0002U
#define BSP_CLK_CLOCK_TYPE_PLL 0x0004U

#define BSP_CLK_CLOCK_STATE_HSE_STATE_DISABLE 0x0000U
#define BSP_CLK_CLOCK_STATE_HSE_STATE_ENABLE 0x0001U
#define BSP_CLK_CLOCK_STATE_HSE_STATE_BYPASS 0x0002U

#define BSP_CLK_CLOCK_STATE_HSI_STATE_DISABLE 0x0000U
#define BSP_CLK_CLOCK_STATE_HSI_STATE_ENABLE 0x0001U

#define BSP_CLK_CLOCK_STATE_PLL_STATE_DISABLE 0x0000U
#define BSP_CLK_CLOCK_STATE_PLL_STATE_ENABLE 0x0001U

#define BSP_CLK_CLOCK_PLL_SRC_NONE 0
#define BSP_CLK_CLOCK_PLL_SRC_HSI RCC_PLLCFGR_PLLSRC_HSI
#define BSP_CLK_CLOCK_PLL_SRC_HSE RCC_PLLCFGR_PLLSRC_HSE


#define BSP_CLK_CLOCK_TYPE_SYSCLK 0x0001U
#define BSP_CLK_CLOCK_TYPE_HCLK 0x0002U
#define BSP_CLK_CLOCK_TYPE_PCLK1 0x0004U
#define BSP_CLK_CLOCK_TYPE_PCLK2 0x0008U

#define BSP_CLK_CLOCK_SOURCE_PLL RCC_CFGR_SW_PLL
#define BSP_CLK_CLOCK_SOURCE_HSE RCC_CFGR_SW_HSE
#define BSP_CLK_CLOCK_SOURCE_HSI RCC_CFGR_SW_HSI

#define IS_HPRE_DIVIDER(VALUE) (((VALUE) == RCC_CFGR_HPRE_DIV1) || ((VALUE) == RCC_CFGR_HPRE_DIV2)|| \
                                ((VALUE) == RCC_CFGR_HPRE_DIV4)|| ((VALUE) == RCC_CFGR_HPRE_DIV8) || \
                                ((VALUE) == RCC_CFGR_HPRE_DIV16)|| ((VALUE) == RCC_CFGR_HPRE_DIV64) || \
                                ((VALUE) == RCC_CFGR_HPRE_DIV128)|| ((VALUE) == RCC_CFGR_HPRE_DIV256) || \
                                ((VALUE) == RCC_CFGR_HPRE_DIV512))


typedef struct
{
    uint32_t PLLState;   /*!< The new state of the PLL.
                            This parameter can be a value of @ref RCC_PLL_Config                      */

    uint32_t PLLSource;  /*!< RCC_PLLSource: PLL entry clock source.
                            This parameter must be a value of @ref RCC_PLL_Clock_Source               */

    uint32_t PLLM;       /*!< PLLM: Division factor for PLL VCO input clock.
                            This parameter must be a value of @ref RCC_PLLM_Clock_Divider             */

    uint32_t PLLN;       /*!< PLLN: Multiplication factor for PLL VCO output clock.
                            This parameter must be a number between Min_Data = 8 and Max_Data = 127    */

    uint32_t PLLP;       /*!< PLLP: Division factor for ADC clock.
                            This parameter must be a value of @ref RCC_PLLP_Clock_Divider             */

    uint32_t PLLQ;       /*!< PLLQ: Division factor for SAI, I2S, USB, FDCAN and QUADSPI clocks.
                            This parameter must be a value of @ref RCC_PLLQ_Clock_Divider             */

    uint32_t PLLR;       /*!< PLLR: Division for the main system clock.
                            User have to set the PLLR parameter correctly to not exceed max frequency 170MHZ.
                            This parameter must be a value of @ref RCC_PLLR_Clock_Divider             */

}bsp_pll_config_t;


typedef struct
{
    uint32_t ClockType;            /*!< The oscillators to be configured.
                                      This parameter can be a value of @ref RCC_Oscillator_Type                   */

    uint32_t HSEState;             /*!< The new state of the HSE.
                                      This parameter can be a value of @ref RCC_HSE_Config                        */

    uint32_t HSIState;             /*!< The new state of the HSI.
                                      This parameter can be a value of @ref RCC_HSI_Config                        */

    bsp_pll_config_t PLL;        /*!< Main PLL structure parameters                                               */

}bsp_clk_osc_config_t;



typedef struct
{
    uint32_t ClockType;
    uint32_t SystemClockSource;
    uint32_t AHBDivider;
    uint32_t APB1Divider;
    uint32_t APB2Divider;

}bsp_clk_clock_config_t;

ret_status BSP_CLK_config_clocks_osc(const bsp_clk_osc_config_t *oscc);

uint32_t BSP_CLK_get_sysclk_freq(void);
uint32_t BSP_CLK_get_hclk_freq(void);
uint32_t BSP_CLK_get_pclk1_freq(void);
uint32_t BSP_CLK_get_pclk2_freq(void);

ret_status BSP_CLK_reset_clocks(void);

#endif  //BSP_CLOCKS_H