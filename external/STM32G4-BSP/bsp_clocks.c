/* Copyright (C) Pablo Rodriguez Nava - All Rights Reserved
 *       * Unauthorized copying of this file, via any medium is strictly prohibited
 *       * Proprietary and confidential
 * Written by Pablo Rodriguez Nava <info@pablintino.com>, June 2021
 */

#include "includes/bsp_clocks.h"
#include "includes/bsp_common_utils.h"
#include "includes/bsp_tick.h"
#include <stddef.h>

/* CMSIS global that contains the current base CPU speed */
uint32_t SystemCoreClock = BSP_CLK_HSI_VALUE;

/** AHB divider values accoring to HPRE possible values. Doesn't contains division by 1 */
static const uint32_t HPRE_DIVIDERS[] = {2, 4, 8, 16, 64, 128, 256, 512};

static const uint32_t PPRE_DIVIDERS[] = {2, 4, 8, 16};

/** Frequency scales for FLASH latency calculation in power range 1 */
static const uint8_t RANGE_1_LATENCY_FREQS[] = {30, 60, 90, 120, 150};

/** Frequency scales for FLASH latency calculation in power range 2 */
static const uint8_t RANGE_2_LATENCY_FREQS[] = {12, 24, 26};

/**
 * Internal functions declarations.
 */
static uint64_t __get_base_pll_freq(void);

static ret_status __config_clock_hse(const bsp_clk_osc_config_t *oscc);

static ret_status __config_clock_pll(const bsp_pll_config_t *pllc);

static uint32_t __calculate_pll_vco_freq(void);

static uint32_t __calculate_pllqclk_freq(void);

static uint32_t __calculate_pllrclk_freq(void);

static uint32_t __calculate_pllpclk_freq(void);

static ret_status __config_clock_hsi(const bsp_clk_osc_config_t *oscc);

static ret_status __pll_validate_params(const bsp_pll_config_t *pllc);

static ret_status __config_clock_hclk(const bsp_clk_clock_config_t *clkc);

static ret_status __config_clock_sysclk(const bsp_clk_clock_config_t *clkc);

static uint8_t __calculate_flash_wait_states(uint32_t frequency);

static uint32_t __calculate_target_hclk_freq(const bsp_clk_clock_config_t *clkc);

static ret_status __change_flash_latency(uint8_t flash_wait_states);

/** Retrieves the current frequency of SYSCLK clock based on register values
    @return The actual SYSCLK in MHz
    */
uint32_t bclk_get_sysclk_freq(void)
{

    switch (RCC->CFGR & RCC_CFGR_SWS) {

    case RCC_CFGR_SWS_PLL: /* PLL used as system clock */
    {
        return __calculate_pllrclk_freq();
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

/** Retrieves the current frequency of HCLK clock based on register values
    @return The actual HCLK in MHz
    */
uint32_t bclk_get_hclk_freq(void)
{
    uint32_t ahb_divider = 1;
    /* If CFGR HPRE MSB bit is set a division factor greater that the unit is applied */
    if (RCC->CFGR & 0x00000080U) {
        /* AHB Clock is divided */
        ahb_divider = HPRE_DIVIDERS[((RCC->CFGR & RCC_CFGR_HPRE) >> RCC_CFGR_HPRE_Pos) & 0x07];
    }
    return bclk_get_sysclk_freq() / ahb_divider;
}

uint32_t bclk_get_pclk1_freq(void)
{
    uint32_t ppre_divider = 1;
    /* If CFGR PPRE MSB bit is set a division factor greater that the unit is applied */
    if (RCC->CFGR & RCC_CFGR_PPRE1_2) {
        /* AHB Clock is divided */
        ppre_divider = PPRE_DIVIDERS[((RCC->CFGR & RCC_CFGR_PPRE1) >> RCC_CFGR_PPRE1_Pos) & 0x03];
    }
    return bclk_get_hclk_freq() / ppre_divider;
}

uint32_t bclk_get_pclk2_freq(void)
{
    uint32_t ppre_divider = 1;
    /* If CFGR PPRE MSB bit is set a division factor greater that the unit is applied */
    if (RCC->CFGR & RCC_CFGR_PPRE2_2) {
        /* AHB Clock is divided */
        ppre_divider = PPRE_DIVIDERS[((RCC->CFGR & RCC_CFGR_PPRE2) >> RCC_CFGR_PPRE2_Pos) & 0x03];
    }
    return bclk_get_hclk_freq() / ppre_divider;
}

uint32_t bclk_get_pllq_freq(void)
{
    return __calculate_pllqclk_freq();
}

/**
 *
 * @return
 */
ret_status bclk_reset_clocks(void)
{

    // Enable HSI
    __BSP_SET_MASKED_REG(RCC->CR, RCC_CR_HSION);

    // Wait until HSI is enabled
    ret_status tmp_status = butil_wait_flag_status_now(&RCC->CR, RCC_CR_HSIRDY, RCC_CR_HSIRDY, BSP_CLK_HSI_READY_TMT);
    if (tmp_status != STATUS_OK) {
        return tmp_status;
    }

    // Change clock source to HSI (PLL can be disabled at the same time, or at least ST does exactly this)
    RCC->CFGR = RCC_CFGR_SW_HSI;
    // Wait until HSI is selected as system clock
    tmp_status = butil_wait_flag_status_now(&RCC->CFGR, RCC_CFGR_SWS, RCC_CFGR_SWS_HSI, 5000U);
    if (tmp_status != STATUS_OK) {
        return tmp_status;
    }

    // Wait until PLL is OFF
    tmp_status = butil_wait_flag_status_now(&RCC->CR, RCC_CR_PLLRDY, 0UL, BSP_CLK_PLL_READY_TMT);
    if (tmp_status != STATUS_OK) {
        return tmp_status;
    }

    // Configure PLL default values
    RCC->PLLCFGR = RCC_PLLCFGR_PLLN_4;

    // Disable RCC interrupts
    RCC->CIER = 0x00000000;

    // Clear reset flags
    __BSP_SET_MASKED_REG(RCC->CSR, RCC_CSR_RMVF);

    // Clear pending interrupts
    RCC->CICR = 0xFFFFFFFFU;

    return STATUS_OK;
}

void bclk_disable_periph_clock(enum bsp_clk_enable_clock clock_disable)
{
    __BSP_BIT_ADDR_OFF_TO_BASE_POINTER_32(RCC_BASE, clock_disable) &= ~__BSP_BIT_ADDR_OFFS_TO_BIT_32(clock_disable);
}

void bclk_enable_periph_clock(enum bsp_clk_enable_clock clock_en)
{
    __BSP_BIT_ADDR_OFF_TO_BASE_POINTER_32(RCC_BASE, clock_en) |= __BSP_BIT_ADDR_OFFS_TO_BIT_32(clock_en);
}

ret_status bclk_config_clocks_osc(const bsp_clk_osc_config_t *oscc)
{
    ret_status temp_status;

    /* Check Null pointer */
    if (oscc == NULL) {
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

ret_status bclk_config_clocks(const bsp_clk_clock_config_t *clkc)
{
    ret_status temp_status;
    uint32_t cfgr_hpre_masked_init_value = RCC->CFGR & RCC_CFGR_HPRE;
    uint32_t target_hclk_freq = __calculate_target_hclk_freq(clkc);

    /* Check Null pointer */
    if (clkc == NULL) {
        return STATUS_ERR;
    }

    /* Check and change if necessary flash latency */
    uint8_t target_latency_waits = __calculate_flash_wait_states(target_hclk_freq);
    if (target_latency_waits > (FLASH->ACR & FLASH_ACR_LATENCY)) {
        temp_status = __change_flash_latency(target_latency_waits);
        if (temp_status != STATUS_OK) {
            return temp_status;
        }
    }

    /*------------------------- SYSCLK Configuration ---------------------------*/
    if (clkc->ClockType & BSP_CLK_CLOCK_TYPE_SYSCLK) {
        temp_status = __config_clock_sysclk(clkc);
        if (temp_status != STATUS_OK) {
            return temp_status;
        }
    }

    /*------------------------- HCLK (AHB Bus main clock) Configuration ---------------------------*/
    if (clkc->ClockType & BSP_CLK_CLOCK_TYPE_HCLK) {
        temp_status = __config_clock_hclk(clkc);
        if (temp_status != STATUS_OK) {
            return temp_status;
        }

    } else if (cfgr_hpre_masked_init_value != (RCC->CFGR & RCC_CFGR_HPRE)) {
        /* HCLK not configured but a 80MHz jump has occurred */
        /* TODO This version simply restores the old value. Take into account that we are not checking that the
            old value violates RCC limits */
        __BSP_SET_MASKED_REG_VALUE(RCC->CFGR, RCC_CFGR_HPRE, cfgr_hpre_masked_init_value);
    }

    /* If we decreased the frequency just decrease latency too if possible */
    if (target_latency_waits < (FLASH->ACR & FLASH_ACR_LATENCY)) {
        temp_status = __change_flash_latency(target_latency_waits);
        if (temp_status != STATUS_OK) {
            return temp_status;
        }
    }

    /* Get the actual configured frequency and call TICK_config to reconfigure SysTick to the current frequency */
    uint32_t final_freq = bclk_get_hclk_freq();
    /* Update the CMSIS clock var */
    SystemCoreClock = final_freq;
    btick_config(final_freq);

    /* Just validate if the desired frequency has been achieved */
    return target_hclk_freq == final_freq ? STATUS_OK : STATUS_ERR;
}

/* CMSIS base CPU speed variable update function */
void SystemCoreClockUpdate(void)
{
    SystemCoreClock = bclk_get_hclk_freq();
}

/*
 *
 * Internal functions. Not meant to be called by external sources.
 *
 */
uint64_t __get_base_pll_freq(void)
{
    uint64_t osc_base_freq = (RCC->PLLCFGR & RCC_PLLCFGR_PLLSRC) == RCC_PLLCFGR_PLLSRC_HSE ? BSP_HSE_VALUE
                                                                                           : BSP_CLK_HSI_VALUE;
    return osc_base_freq / (((RCC->PLLCFGR & RCC_PLLCFGR_PLLM) >> RCC_PLLCFGR_PLLM_Pos) + 1);
}

ret_status __config_clock_hsi(const bsp_clk_osc_config_t *oscc)
{

    if (!IS_HSI_STATE(oscc->HSIState)) {
        return STATUS_ERR;
    }

    /* Cannot change HSI state when its being used by system clock */
    if ((RCC->CFGR & RCC_CFGR_SWS) == RCC_CFGR_SWS_HSI ||
        (((RCC->CFGR & RCC_CFGR_SWS) == RCC_CFGR_SWS_PLL) &&
         ((RCC->PLLCFGR & RCC_PLLCFGR_PLLSRC) == RCC_PLLCFGR_PLLSRC_HSI))) {
        return STATUS_ERR;
    }

    if (oscc->HSIState == BSP_CLK_CLOCK_STATE_HSI_STATE_ENABLE) {
        /* Enable HSI */
        __BSP_SET_MASKED_REG(RCC->CR, RCC_CR_HSION);
        return butil_wait_flag_status_now(&RCC->CR, RCC_CR_HSIRDY, RCC_CR_HSIRDY, BSP_CLK_HSI_READY_TMT);

    } else if (oscc->HSIState == BSP_CLK_CLOCK_STATE_HSI_STATE_DISABLE) {
        /* Disable HSI oscillator */
        __BSP_CLEAR_MASKED_REG(RCC->CR, RCC_CR_HSION);

        /* Wait for HSI to turn off */
        return butil_wait_flag_status_now(&RCC->CR, RCC_CR_HSIRDY, 0UL, BSP_CLK_HSI_READY_TMT);
    } else {
        /* Invalid configuration option. Shouldn't be reached */
        return STATUS_ERR;
    }
}

ret_status __config_clock_hse(const bsp_clk_osc_config_t *oscc)
{

    if (!IS_HSE_STATE(oscc->HSEState)) {
        return STATUS_ERR;
    }

    /* Cannot change HSE state when its being used by system clock */
    if ((RCC->CFGR & RCC_CFGR_SWS) == RCC_CFGR_SWS_HSE ||
        (((RCC->CFGR & RCC_CFGR_SWS) == RCC_CFGR_SWS_PLL) &&
         ((RCC->PLLCFGR & RCC_PLLCFGR_PLLSRC) == RCC_PLLCFGR_PLLSRC_HSE))) {
        return STATUS_ERR;
    }

    if (oscc->HSEState == BSP_CLK_CLOCK_STATE_HSE_STATE_ENABLE) {
        /* Enable HSE */
        __BSP_SET_MASKED_REG(RCC->CR, RCC_CR_HSEON);
        __BSP_CLEAR_MASKED_REG(RCC->CR, RCC_CR_HSEBYP);

        /* Wait for HSE to turn on */
        // TODO Remove hardcoded timeout
        return butil_wait_flag_status_now(&RCC->CR, RCC_CR_HSERDY, RCC_CR_HSERDY, BSP_CLK_HSE_READY_TMT);

    } else if (oscc->HSEState == BSP_CLK_CLOCK_STATE_HSE_STATE_DISABLE) {
        /* Disable HSE oscillator */
        __BSP_CLEAR_MASKED_REG(RCC->CR, RCC_CR_HSEON);

        /* Set bypass HSE flag if HSE bypass mode is selected */
        RCC->CR ^= (-(oscc->HSEState == BSP_CLK_CLOCK_STATE_HSE_STATE_BYPASS) ? 1U : 0) & (1U << RCC_CR_HSEBYP_Pos);

        /* Wait for HSE to turn off */
        return butil_wait_flag_status_now(&RCC->CR, RCC_CR_HSERDY, 0, BSP_CLK_HSE_READY_TMT);
    } else {
        /* Invalid configuration option. Shouldn't be reached */
        return STATUS_ERR;
    }
}

ret_status __disable_pll()
{
    /* Disable the PLL */
    __BSP_CLEAR_MASKED_REG(RCC->CR, RCC_CR_PLLON);

    /* Wait until PLL is disabled */
    return butil_wait_flag_status_now(&RCC->CR, RCC_CR_PLLRDY, 0UL, BSP_CLK_PLL_READY_TMT);
}

ret_status __pll_validate_params(const bsp_pll_config_t *pllc)
{

    /* Validate PLL SRC type */
    if (pllc->PLLSource != BSP_CLK_CLOCK_PLL_SRC_NONE && pllc->PLLSource != BSP_CLK_CLOCK_PLL_SRC_HSE &&
        pllc->PLLSource != BSP_CLK_CLOCK_PLL_SRC_HSI) {
        return STATUS_ERR;
    }

    /* Validate PLL multipliers */
    if (!BSP_CLK_IS_PLLP_VALID(pllc->PLLP) || !BSP_CLK_IS_PLLQ_VALID(pllc->PLLQ) ||
        !BSP_CLK_IS_PLLR_VALID(pllc->PLLR) || !BSP_CLK_IS_PLLM_VALID(pllc->PLLM) ||
        !BSP_CLK_IS_PLLN_VALID(pllc->PLLN)) {
        return STATUS_ERR;
    }

    /* PLL input freq must be below 16MHz and above 2.66MHz */
    uint32_t pll_input_freq = pllc->PLLSource == BSP_CLK_CLOCK_PLL_SRC_HSI ? BSP_CLK_HSI_VALUE : BSP_HSE_VALUE;
    if (pll_input_freq / pllc->PLLM > 16000000UL || pll_input_freq / pllc->PLLM < 2666666UL) {
        return STATUS_ERR;
    }

    /* PLL VCO output frequency must be between 96 and 344MHz */
    uint32_t pll_vco_output_freq = pll_input_freq * pllc->PLLN / pllc->PLLM;
    if (pll_vco_output_freq > 344000000UL || pll_vco_output_freq < 96000000UL) {
        return STATUS_ERR;
    }

    /* PLL P output frequency must be between 2.0645 and 170MHz */
    uint32_t pll_p_freq = pll_vco_output_freq / pllc->PLLP;
    if (pll_p_freq > 170000000UL || pll_p_freq < 2064500UL) {
        return STATUS_ERR;
    }

    /* PLL P output frequency must be between 8 and 170MHz */
    uint32_t pll_q_freq = pll_vco_output_freq / pllc->PLLQ;
    if (pll_q_freq > 170000000UL || pll_q_freq < 8000000UL) {
        return STATUS_ERR;
    }

    /* PLL P output frequency must be between 8 and 170MHz */
    uint32_t pll_r_freq = pll_vco_output_freq / pllc->PLLR;
    if (pll_r_freq > 170000000UL || pll_r_freq < 8000000UL) {
        return STATUS_ERR;
    }

    return STATUS_OK;
}

ret_status __config_clock_pll(const bsp_pll_config_t *pllc)
{
    ret_status tmp_status;
    uint32_t tmp_pllcfgr;

    if (pllc == NULL) {
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

        /* Obtain a copy of the PLL config register */
        tmp_pllcfgr = RCC->PLLCFGR;

        /* Clear current values for multipliers/dividers and PLLSource */
        tmp_pllcfgr &= ~(RCC_PLLCFGR_PLLN | RCC_PLLCFGR_PLLM | RCC_PLLCFGR_PLLP | RCC_PLLCFGR_PLLQ | RCC_PLLCFGR_PLLR |
                         RCC_PLLCFGR_PLLSRC);

        /* Assign PLL multipliers and dividers (a translation to register values is done previous assigment) */
        tmp_pllcfgr |= (((pllc->PLLM - 1U) << RCC_PLLCFGR_PLLM_Pos) & RCC_PLLCFGR_PLLM) |
                       ((pllc->PLLN << RCC_PLLCFGR_PLLN_Pos) & RCC_PLLCFGR_PLLN) |
                       ((pllc->PLLP << RCC_PLLCFGR_PLLPDIV_Pos) & RCC_PLLCFGR_PLLPDIV) |
                       ((((pllc->PLLQ >> 1U) - 1U) << RCC_PLLCFGR_PLLQ_Pos) & RCC_PLLCFGR_PLLQ) |
                       ((((pllc->PLLR >> 1U) - 1U) << RCC_PLLCFGR_PLLR_Pos) & RCC_PLLCFGR_PLLR);

        /* Set PLL clock source */
        tmp_pllcfgr |= pllc->PLLSource;

        /* Reconfigure PLL with the previously crafted multipliers/dividers and PLL source */
        RCC->PLLCFGR = tmp_pllcfgr;

        /* Enable the PLL */
        __BSP_SET_MASKED_REG(RCC->CR, RCC_CR_PLLON);

        /* Wait until PLL is enabled */
        return butil_wait_flag_status_now(&RCC->CR, RCC_CR_PLLRDY, RCC_CR_PLLRDY, BSP_CLK_PLL_READY_TMT);

    } else if (pllc->PLLState == BSP_CLK_CLOCK_STATE_PLL_STATE_DISABLE) {
        return __disable_pll();
    } else {
        /* Invalid configuration option. Shouldn't be reached */
        return STATUS_ERR;
    }
}

ret_status __config_clock_sysclk(const bsp_clk_clock_config_t *clkc)
{

    /* Validate input clock source to be PLL, HSE or HSI */
    if (!IS_CLOCK_SOURCE(clkc->SystemClockSource)) {
        return STATUS_ERR;
    }

    /* If selected source is already applied do nothing */
    if ((RCC->CFGR & RCC_CFGR_SWS) == clkc->SystemClockSource) {
        return STATUS_OK;
    }

    /* PLL is selected as System Clock Source */
    if (clkc->SystemClockSource == BSP_CLK_CLOCK_SOURCE_PLL) {
        /* If PLL is not ready (and turned on) just return a error */
        if (!(RCC->CR & RCC_CR_PLLRDY)) {
            return STATUS_ERR;
        }

        /* As stated in RM0440, chapter 7.2.7: For clock jumps above 80MHz an intermediate jump with AHB/2 clock should
         * be made */
        /* Notice that que assume (cannot come from PLL) that we came from HSI/HSE lower frequencies */
        uint32_t pll_output_freq = __calculate_pllrclk_freq();
        if (pll_output_freq > 80000000U) {
            __BSP_SET_MASKED_REG_VALUE(RCC->CFGR, RCC_CFGR_HPRE, RCC_CFGR_HPRE_DIV2);
            uint32_t temp_status = butil_wait_flag_status_now(&RCC->CFGR, RCC_CFGR_HPRE, RCC_CFGR_HPRE_DIV2, 100UL);
            if (temp_status != STATUS_OK) {
                return temp_status;
            }
        }

        /* Enable SYSCLK PLL output (R output) */
        __BSP_SET_MASKED_REG(RCC->PLLCFGR, RCC_PLLCFGR_PLLREN);

    } else {
        /* HSE or HSI selected as System Clock Source. Be sure that the source OSC is ready */
        if (((clkc->SystemClockSource == BSP_CLK_CLOCK_SOURCE_HSE) && !(RCC->CR & RCC_CR_HSERDY)) ||
            ((clkc->SystemClockSource == BSP_CLK_CLOCK_SOURCE_HSI) && !(RCC->CR & RCC_CR_HSIRDY))) {
            return STATUS_ERR;
        }

        /* Same case as jumps over 80MHz barrier. If target frequency goes under 80MHz a secondary
         * frequency jump with AHB/2 is necessary. See RM0440, chapter 7.2.7 */
        /* Notice that this case can only happen when swtiching from PLL */
        uint32_t pll_output_freq = bclk_get_sysclk_freq();
        if (pll_output_freq > 80000000U) {
            __BSP_SET_MASKED_REG_VALUE(RCC->CFGR, RCC_CFGR_HPRE, RCC_CFGR_HPRE_DIV2);
            uint32_t temp_status = butil_wait_flag_status_now(&RCC->CFGR, RCC_CFGR_HPRE, RCC_CFGR_HPRE_DIV2, 100UL);
            if (temp_status != STATUS_OK) {
                return temp_status;
            }
        }
    }

    /* Set the specified clock source */
    __BSP_SET_MASKED_REG_VALUE(RCC->CFGR, RCC_CFGR_SW, clkc->SystemClockSource);

    return butil_wait_flag_status_now(&RCC->CFGR, RCC_CFGR_SWS, clkc->SystemClockSource << RCC_CFGR_SWS_Pos, 5000UL);
}

ret_status __config_clock_hclk(const bsp_clk_clock_config_t *clkc)
{

    if (!IS_HPRE_DIVIDER(clkc->AHBDivider) || !IS_APB1_DIVIDER(clkc->APB1_prescaler) ||
        !IS_APB2_DIVIDER(clkc->APB2_prescaler)) {
        return STATUS_ERR;
    }

    // TODO As first approach just set the highest prescaler in order to not exceed the top frequency limit of the three
    // buses
    //  This assumes that the user always indicates the three values
    RCC->CFGR = (RCC->CFGR & ~RCC_CFGR_PPRE1) | RCC_CFGR_PPRE1_DIV16;
    RCC->CFGR = (RCC->CFGR & ~RCC_CFGR_PPRE2) | RCC_CFGR_PPRE2_DIV16;

    // Just assign the divider. It must be already shifted to the HPRE bits position
    __BSP_SET_MASKED_REG_VALUE(RCC->CFGR, RCC_CFGR_HPRE, clkc->AHBDivider);

    uint32_t temp_status = butil_wait_flag_status_now(&RCC->CFGR, RCC_CFGR_HPRE, clkc->AHBDivider, 100UL);
    if (temp_status != STATUS_OK) {
        return temp_status;
    }

    /* Assign APB1 and APB2 bus clock dividers */
    RCC->CFGR = (RCC->CFGR & ~(RCC_CFGR_PPRE1 | RCC_CFGR_PPRE2)) | clkc->APB1_prescaler | clkc->APB2_prescaler;

    return butil_wait_flag_status_now(
        &RCC->CFGR, RCC_CFGR_PPRE1 | RCC_CFGR_PPRE2, clkc->APB1_prescaler | clkc->APB2_prescaler, 100UL);
}

uint32_t __calculate_target_hclk_freq(const bsp_clk_clock_config_t *clkc)
{
    uint32_t base_freq = BSP_CLK_HSI_VALUE;
    if (clkc->SystemClockSource == BSP_CLK_CLOCK_SOURCE_PLL) {
        base_freq = __calculate_pllrclk_freq();
    } else if (clkc->SystemClockSource == BSP_CLK_CLOCK_SOURCE_HSE) {
        base_freq = __calculate_pllrclk_freq();
    } else if (clkc->SystemClockSource == BSP_CLK_CLOCK_SOURCE_HSI) {
        base_freq = BSP_CLK_HSI_VALUE;
    }

    /* If HPRE MSB is set division factor is greater than one */
    return base_freq / (clkc->AHBDivider & 0x00000080U
                            ? HPRE_DIVIDERS[((clkc->AHBDivider & RCC_CFGR_HPRE) >> RCC_CFGR_HPRE_Pos) & 0x03]
                            : 1UL);
}

uint32_t __calculate_pllpclk_freq(void)
{
    return __calculate_pll_vco_freq() / ((uint8_t)((RCC->PLLCFGR & RCC_PLLCFGR_PLLPDIV) >> RCC_PLLCFGR_PLLPDIV_Pos));
}

uint32_t __calculate_pllrclk_freq(void)
{
    return __calculate_pll_vco_freq() /
           ((uint8_t)(((RCC->PLLCFGR & RCC_PLLCFGR_PLLR) >> RCC_PLLCFGR_PLLR_Pos) + 1) << 1);
}

uint32_t __calculate_pllqclk_freq(void)
{
    return __calculate_pll_vco_freq() /
           ((uint8_t)(((RCC->PLLCFGR & RCC_PLLCFGR_PLLQ) >> RCC_PLLCFGR_PLLQ_Pos) + 1) << 1);
}

uint32_t __calculate_pll_vco_freq(void)
{
    uint32_t pllmul = (uint8_t)((RCC->PLLCFGR & RCC_PLLCFGR_PLLN) >> RCC_PLLCFGR_PLLN_Pos);

    return __get_base_pll_freq() * pllmul;
}

uint8_t __calculate_flash_wait_states(uint32_t frequency)
{
    uint32_t freq_mhz = frequency / 1000000UL;
    uint8_t latencies = (PWR->CR1 & PWR_CR1_VOS) == PWR_CR1_VOS_0 ? BSP_UTL_COUNT_OF(RANGE_1_LATENCY_FREQS)
                                                                  : BSP_UTL_COUNT_OF(RANGE_2_LATENCY_FREQS);

    for (int latency = 0; latency < latencies; latency++) {
        if (freq_mhz <= ((PWR->CR1 & PWR_CR1_VOS) == PWR_CR1_VOS_0 ? RANGE_1_LATENCY_FREQS[latency]
                                                                   : RANGE_2_LATENCY_FREQS[latency])) {
            return latency;
        }
    }

    return FLASH_ACR_LATENCY_0WS;
}

ret_status __change_flash_latency(uint8_t flash_wait_states)
{
    __BSP_SET_MASKED_REG_VALUE(FLASH->ACR, FLASH_ACR_LATENCY, flash_wait_states);
    return butil_wait_flag_status_now(&FLASH->ACR, FLASH_ACR_LATENCY, flash_wait_states, 10UL);
}
