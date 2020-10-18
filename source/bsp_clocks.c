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


#include "bsp_clocks.h"
#include "bsp_tick.h"

#define __BSP_CLK_WAIT_FLAG_CLEAR 0
#define __BSP_CLK_WAIT_FLAG_SET 0x0001U


/*
 * Internal functions declarations.
 */
static uint64_t __get_base_pll_freq(void);
static ret_status __wait_flag_status(volatile uint32_t *reg, uint32_t flag, uint8_t wait_ready, uint32_t timeout);
static ret_status __config_clock_hse(const bsp_clk_osc_config_t *oscc);
static ret_status __config_clock_pll(const bsp_pll_config_t *pllc);
static uint32_t __calculate_pll_freq(void);
static ret_status __config_clock_hsi(const bsp_clk_osc_config_t *oscc);
static ret_status __pll_validate_params(const bsp_pll_config_t *pllc);
static ret_status __config_clock_hclk(const bsp_clk_clock_config_t *clkc);
static ret_status __config_clock_sysclk(const bsp_clk_clock_config_t *clkc);



/* Used to give the current frequency even if not set by BSP_configure_system_clock */
uint32_t BSP_CLK_get_sysclk_freq(void) {


    switch (RCC->CFGR & RCC_CFGR_SWS) {

        case RCC_CFGR_SWS_PLL:  /* PLL used as system clock */
        {
            return __calculate_pll_freq();
        }
        case RCC_CFGR_SWS_HSE:
            return BSP_HSE_VALUE;
        case RCC_CFGR_SWS_HSI:
            return BSP_CLK_HSI_VALUE;
        default:
            // TODO Need to report an error for this case
            return BSP_CLK_HSI_VALUE;
    }
}


ret_status BSP_CLK_reset_clocks(void) {
    ret_status tmp_status;

    //Enable HSI
    RCC->CR |= RCC_CR_HSION;
    // Wait until HSI is enabled
    tmp_status = __wait_flag_status(&RCC->CR, RCC_CR_HSIRDY, __BSP_CLK_WAIT_FLAG_SET, BSP_CLK_HSI_READY_TMT);
    if (tmp_status != STATUS_OK) {
        return tmp_status;
    }

    //Change clock source to HSI (PLL can be disabled at the same time, or at least ST does exactly this)
    RCC->CFGR = RCC_CFGR_SW_HSI;
    // Wait until HSI is selected as system clock
    while ((RCC->CFGR & RCC_CFGR_SWS) != RCC_CFGR_SWS_HSI);

    // Wait until PLL is OFF
    tmp_status = __wait_flag_status(&RCC->CR, RCC_CR_PLLRDY, __BSP_CLK_WAIT_FLAG_CLEAR, BSP_CLK_PLL_READY_TMT);
    if (tmp_status != STATUS_OK) {
        return tmp_status;
    }

    // Configure PLL default values
    RCC->PLLCFGR = RCC_PLLCFGR_PLLN_4;

    // Disable RCC interrupts
    RCC->CIER = 0x00000000;

    // Clear reset flags
    RCC->CSR |= RCC_CSR_RMVF;

    // Clear pending interrupts
    RCC->CICR = 0xFFFFFFFFU;

    return STATUS_OK;
}


ret_status BSP_CLK_config_clocks_osc(const bsp_clk_osc_config_t *oscc) {
    ret_status temp_status;

    /* Check Null pointer */
    if (!oscc) {
        return STATUS_ERR;
    }

    /*------------------------------- HSE Configuration ------------------------*/
    if (oscc->ClockType & BSP_CLK_CLOCK_TYPE_HSE) {
        temp_status = __config_clock_hse(oscc);
        if (temp_status != STATUS_OK) {
            return temp_status;
        }
    }

    /*------------------------------- HSI Configuration ------------------------*/
    if (oscc->ClockType & BSP_CLK_CLOCK_TYPE_HSI) {
        temp_status = __config_clock_hsi(oscc);
        if (temp_status != STATUS_OK) {
            return temp_status;
        }
    }

    /*------------------------------- PLL Configuration ------------------------*/
    if (oscc->ClockType & BSP_CLK_CLOCK_TYPE_PLL) {
        return __config_clock_pll(&oscc->PLL);
    }

    return STATUS_OK;
}


ret_status BSP_CLK_config_clocks(const bsp_clk_clock_config_t *clkc) {
    ret_status temp_status;

    /* Check Null pointer */
    if (!clkc) {
        return STATUS_ERR;
    }

    /*------------------------- SYSCLK Configuration ---------------------------*/
    if (clkc->ClockType & BSP_CLK_CLOCK_TYPE_SYSCLK) {
        temp_status = __config_clock_sysclk(clkc);
        if (temp_status != STATUS_OK){
            return temp_status;
        }
    }

    /*------------------------- HCLK (AHB Bus main clock) Configuration ---------------------------*/
    if (clkc->ClockType & BSP_CLK_CLOCK_TYPE_HCLK) {
        temp_status = __config_clock_hclk(clkc);
        if (temp_status != STATUS_OK){
            return temp_status;
        }
    }

    BSP_TICK_config(BSP_CLK_get_hclk_freq());

    return temp_status;
}


/*
 *
 * Internal functions. Not to be called by external sources.
 *
 */
uint64_t __get_base_pll_freq(void) {
    return (RCC->PLLCFGR & RCC_PLLCFGR_PLLSRC) == RCC_PLLCFGR_PLLSRC_HSE ? BSP_HSE_VALUE : BSP_CLK_HSI_VALUE;
}


ret_status __wait_flag_status(volatile uint32_t *reg, uint32_t flag, uint8_t wait_ready, uint32_t timeout) {
    uint32_t tickstart = BSP_TICK_get_ticks();

    if (wait_ready != __BSP_CLK_WAIT_FLAG_CLEAR && wait_ready != __BSP_CLK_WAIT_FLAG_SET) {
        return STATUS_ERR;
    }

    while ((wait_ready == __BSP_CLK_WAIT_FLAG_CLEAR && (*reg & flag)) ||
           (wait_ready != __BSP_CLK_WAIT_FLAG_CLEAR && (!(*reg) & flag))) {
        if (BSP_TICK_get_ticks() - tickstart > timeout) {
            return STATUS_TMT;
        }
    }
    return STATUS_OK;
}


ret_status __config_clock_hsi(const bsp_clk_osc_config_t *oscc) {
    /* Cannot change HSI state when its being used by system clock */
    if ((RCC->CFGR & RCC_CFGR_SWS) == RCC_CFGR_SWS_HSI || (((RCC->CFGR & RCC_CFGR_SWS) == RCC_CFGR_SWS_PLL) &&
                                                           ((RCC->PLLCFGR & RCC_PLLCFGR_PLLSRC) ==
                                                            RCC_PLLCFGR_PLLSRC_HSI))) {
        return STATUS_ERR;
    }

    if (oscc->HSIState == BSP_CLK_CLOCK_STATE_HSI_STATE_ENABLE) {
        /* Enable HSI */
        RCC->CR |= RCC_CR_HSION;
        //TODO Remove hardcoded timeout
        return __wait_flag_status(&RCC->CR, RCC_CR_HSIRDY, __BSP_CLK_WAIT_FLAG_SET, BSP_CLK_HSI_READY_TMT);

    } else {
        /* Disable HSI oscillator */
        RCC->CR &= ~RCC_CR_HSION;

        /* Wait for HSI to turn off */
        //TODO Remove hardcoded timeout
        return __wait_flag_status(&RCC->CR, RCC_CR_HSIRDY, __BSP_CLK_WAIT_FLAG_CLEAR, BSP_CLK_HSI_READY_TMT);
    }
}


ret_status __config_clock_hse(const bsp_clk_osc_config_t *oscc) {

    /* Cannot change HSE state when its being used by system clock */
    if ((RCC->CFGR & RCC_CFGR_SWS) == RCC_CFGR_SWS_HSE || (((RCC->CFGR & RCC_CFGR_SWS) == RCC_CFGR_SWS_PLL) &&
                                                           ((RCC->PLLCFGR & RCC_PLLCFGR_PLLSRC) ==
                                                            RCC_PLLCFGR_PLLSRC_HSE))) {
        return STATUS_ERR;
    }

    if (oscc->HSEState == BSP_CLK_CLOCK_STATE_HSE_STATE_ENABLE) {
        /* Enable HSE */
        RCC->CR |= RCC_CR_HSEON;
        RCC->CR &= ~RCC_CR_HSEBYP;

        /* Wait for HSE to turn on */
        //TODO Remove hardcoded timeout
        return __wait_flag_status(&RCC->CR, RCC_CR_HSERDY, __BSP_CLK_WAIT_FLAG_SET, BSP_CLK_HSE_READY_TMT);

    } else {
        /* Disable HSE oscillator */
        RCC->CR &= ~RCC_CR_HSEON;
        /* Set bypass HSE flag if HSE bypass mode is selected */
        RCC->CR ^= (-(oscc->HSEState == BSP_CLK_CLOCK_STATE_HSE_STATE_BYPASS) ? 1U : 0) &
                   (1U << RCC_CR_HSEBYP_Pos);

        /* Wait for HSE to turn off */
        //TODO Remove hardcoded timeout
        return __wait_flag_status(&RCC->CR, RCC_CR_HSERDY, __BSP_CLK_WAIT_FLAG_CLEAR, BSP_CLK_HSE_READY_TMT);
    }
}


ret_status __disable_pll() {
    /* Disable the PLL */
    RCC->CR &= ~RCC_CR_PLLON;

    /* Wait until PLL is disabled */
    return __wait_flag_status(&RCC->CR, RCC_CR_PLLRDY, __BSP_CLK_WAIT_FLAG_CLEAR, BSP_CLK_PLL_READY_TMT);
}


ret_status __pll_validate_params(const bsp_pll_config_t *pllc) {
    uint32_t pll_input_freq;
    uint32_t pll_vco_output_freq;
    uint32_t pll_q_freq;
    uint32_t pll_p_freq;
    uint32_t pll_r_freq;

    /* Validate PLL SRC type */
    if (pllc->PLLSource != BSP_CLK_CLOCK_PLL_SRC_NONE ||
        pllc->PLLSource != BSP_CLK_CLOCK_PLL_SRC_HSE ||
        pllc->PLLSource != BSP_CLK_CLOCK_PLL_SRC_HSI) {
        return STATUS_ERR;
    }

    /* PLL input freq must be below 16MHz and above 2.66MHz */
    pll_input_freq = pllc->PLLSource == BSP_CLK_CLOCK_PLL_SRC_HSI ? BSP_CLK_HSI_VALUE : BSP_HSE_VALUE;
    if (pll_input_freq / pllc->PLLM > 16000000UL ||
        pll_input_freq / pllc->PLLM < 2666666UL) {
        return STATUS_ERR;
    }

    /* PLL VCO output frequency must be between 96 and 344MHz */
    pll_vco_output_freq = pll_input_freq * pllc->PLLN / pllc->PLLM;
    if (pll_vco_output_freq > 344000000UL || pll_vco_output_freq < 96000000UL) {
        return STATUS_ERR;
    }

    /* PLL P output frequency must be between 2.0645 and 170MHz */
    pll_p_freq = pll_vco_output_freq * pllc->PLLP;
    if (pll_p_freq > 170000000UL || pll_p_freq < 2064500UL) {
        return STATUS_ERR;
    }

    /* PLL P output frequency must be between 8 and 170MHz */
    pll_q_freq = pll_vco_output_freq * pllc->PLLQ;
    if (pll_q_freq > 170000000UL || pll_q_freq < 8000000UL) {
        return STATUS_ERR;
    }

    /* PLL P output frequency must be between 8 and 170MHz */
    pll_r_freq = pll_vco_output_freq * pllc->PLLR;
    if (pll_r_freq > 170000000UL || pll_r_freq < 8000000UL) {
        return STATUS_ERR;
    }

    return STATUS_OK;
}


ret_status __config_clock_pll(const bsp_pll_config_t *pllc) {
    ret_status tmp_status;
    uint32_t tmp_pllcfgr;

    if (!pllc) {
        return STATUS_ERR;
    }

    /* PLL cannot be configured if its being used by system clock */
    if ((RCC->CFGR & RCC_CFGR_SWS) == RCC_CFGR_SWS_PLL) {
        return STATUS_ERR;
    }

    if (pllc->PLLState == BSP_CLK_CLOCK_STATE_PLL_STATE_ENABLE) {

        /* Validate PLL parameters before configuring */
        tmp_status = __pll_validate_params(pllc);
        if (tmp_status != STATUS_OK) {
            return tmp_status;
        }

        /* To configure PLL divider and multiplier the whole PLL should be turned off */
        tmp_status = __disable_pll();
        if (tmp_status != STATUS_OK) {
            return tmp_status;
        }

        tmp_pllcfgr = RCC->PLLCFGR;
        tmp_pllcfgr &= ~(RCC_PLLCFGR_PLLN | RCC_PLLCFGR_PLLM | RCC_PLLCFGR_PLLP | RCC_PLLCFGR_PLLQ | RCC_PLLCFGR_PLLR |
                         RCC_PLLCFGR_PLLSRC_Msk);
        tmp_pllcfgr |= (((pllc->PLLM - 1U) & RCC_PLLCFGR_PLLM_Msk) << RCC_PLLCFGR_PLLM_Pos) |
                       ((pllc->PLLN & RCC_PLLCFGR_PLLN_Msk) << RCC_PLLCFGR_PLLN_Pos) |
                       ((pllc->PLLP & RCC_PLLCFGR_PLLP_Msk) << RCC_PLLCFGR_PLLP_Pos) |
                       ((((pllc->PLLQ >> 1U) - 1U) & RCC_PLLCFGR_PLLQ_Msk) << RCC_PLLCFGR_PLLQ_Pos) |
                       ((((pllc->PLLR >> 1U) - 1U) & RCC_PLLCFGR_PLLR_Msk) << RCC_PLLCFGR_PLLR_Pos);

        /* Set PLL clock source */
        tmp_pllcfgr |= pllc->PLLSource;

        RCC->PLLCFGR = tmp_pllcfgr;

        /* Enable the PLL */
        RCC->CR |= RCC_CR_PLLON;

        /* Wait until PLL is enabled */
        return __wait_flag_status(&RCC->CR, RCC_CR_PLLRDY, __BSP_CLK_WAIT_FLAG_SET, BSP_CLK_PLL_READY_TMT);

    } else {
        return __disable_pll();
    }

}


uint32_t __calculate_pll_freq(void) {
    uint32_t pllmul = (uint8_t) (RCC->PLLCFGR & RCC_PLLCFGR_PLLN) >> RCC_PLLCFGR_PLLN_Pos;
    uint8_t pll_main_div = (uint8_t) (RCC->PLLCFGR & RCC_PLLCFGR_PLLPDIV) >> RCC_PLLCFGR_PLLPDIV_Pos;
    uint8_t pll_system_clk_div = (uint8_t) (RCC->PLLCFGR & RCC_PLLCFGR_PLLR) >> RCC_PLLCFGR_PLLR_Pos;
    return (uint32_t) (__get_base_pll_freq() / (uint64_t) (pll_main_div + pll_system_clk_div)) *
           ((uint64_t) pllmul);
}

ret_status __config_clock_sysclk(const bsp_clk_clock_config_t *clkc) {
    uint32_t pll_output_freq;

    /* Validate input clock source to be PLL, HSE or HSI */
    if (clkc->SystemClockSource != RCC_CFGR_SW_PLL && clkc->SystemClockSource != RCC_CFGR_SW_HSE &&
        clkc->SystemClockSource != RCC_CFGR_SW_HSI) {
        return STATUS_ERR;
    }

    /* PLL is selected as System Clock Source */
    if (clkc->SystemClockSource == BSP_CLK_CLOCK_SOURCE_PLL) {
        /* If PLL is not ready (and turned on) just return a error */
        if (!(RCC->CR & RCC_CR_PLLRDY)) {
            return STATUS_ERR;
        }

        /* As stated in RM0440, chapter 7.2.7: For clock jumps above 80MHz an intermediate jump with AHB/2 clock should be made */
        pll_output_freq = __calculate_pll_freq();
        if (pll_output_freq > 80000000U) {
            RCC->CFGR = (RCC->CFGR & ~RCC_CFGR_HPRE) | RCC_CFGR_HPRE_DIV2;
        }
    } else {
        /* HSE or HSE selected as System Clock Source. Be sure that the source OSC is ready */
        if (((clkc->SystemClockSource == BSP_CLK_CLOCK_SOURCE_HSE) && !(RCC->CR & RCC_CR_HSERDY)) ||
            ((clkc->SystemClockSource == BSP_CLK_CLOCK_SOURCE_HSI) && !(RCC->CR & RCC_CR_HSIRDY))) {
            return STATUS_ERR;
        }

        /* Same case as jumps over 80MHz barrier. If target frequency goes under 80MHz a secondary
         * frequency jump with AHB/2 is neccesary. See RM0440, chapter 7.2.7 */
        pll_output_freq = BSP_CLK_get_sysclk_freq();
        if (pll_output_freq > 80000000U) {
            RCC->CFGR = (RCC->CFGR & ~RCC_CFGR_HPRE) | RCC_CFGR_HPRE_DIV2;
        }
    }

    /* Set the specified clock source */
    RCC->CFGR = (RCC->CFGR & ~RCC_CFGR_SW) | clkc->SystemClockSource;
    while ((RCC->CFGR & RCC_CFGR_SWS) != RCC_CFGR_SWS_HSI);

    return STATUS_OK;
}


ret_status __config_clock_hclk(const bsp_clk_clock_config_t *clkc) {
    if(!IS_HPRE_DIVIDER(clkc->AHBDivider)){
        return STATUS_ERR;
    }

    //TODO As first approach just set the highest prescaler in order to not exceed the top frequency limit of the three buses
    // This assumes that the user always indicates the three values
    RCC->CFGR = (RCC->CFGR & ~RCC_CFGR_PPRE1) | RCC_CFGR_PPRE1_DIV16;
    RCC->CFGR = (RCC->CFGR & ~RCC_CFGR_PPRE2) | RCC_CFGR_PPRE2_DIV16;

    // Just assign the divider. It must be already shifted to the HPRE bits position
    RCC->CFGR = (RCC->CFGR & ~RCC_CFGR_HPRE) | clkc->AHBDivider;

    //TODO Check that the change success (If the written value is not set after some cycles it means that the HW has
    // rejected the value)
    return STATUS_OK;
}