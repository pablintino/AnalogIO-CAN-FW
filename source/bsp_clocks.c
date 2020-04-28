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


/* Current system frequency */
uint32_t sys_frequency = 0UL;


static const uint8_t rcc_pll_multipliers[16] = {2U, 3U, 4U, 5U, 6U, 7U, 8U, 9U, 10U, 11U, 12U, 13U, 14U, 15U, 16U, 16U};
static const uint8_t rcc_pll_dividers[16] = {1U, 2U, 3U, 4U, 5U, 6U, 7U, 8U, 9U, 10U, 11U, 12U, 13U, 14U, 15U, 16U};

static void _config_rcc_clocks(void);

/*
 * Configure system clock and sets periodic SysTick Timer and its IRQ
 */
void BSP_configure_clocks(void) {
    _config_rcc_clocks();

    sys_frequency = BSP_get_system_freq();

    SysTick_Config(sys_frequency / (uint32_t) BSP_SYSTICK_RATE);
}

/* Used to give the current frequency even if not set by BSP_configure_system_clock */
uint32_t BSP_get_system_freq(void) {
    uint32_t cfgr_reg;
    uint32_t prediv;
    uint32_t pllclk;
    uint32_t pllmul;
    uint32_t sysclockfreq;

    cfgr_reg = RCC->CFGR;

    switch (cfgr_reg & RCC_CFGR_SWS) {

        case RCC_CFGR_SWS_PLL:  /* PLL used as system clock */
        {
            pllmul = rcc_pll_multipliers[(uint32_t) (cfgr_reg & RCC_CFGR_PLLMUL) >> POSITION_VAL(RCC_CFGR_PLLMUL)];
            prediv = rcc_pll_dividers[(uint32_t) (RCC->CFGR2 & RCC_CFGR2_PREDIV) >> POSITION_VAL(RCC_CFGR2_PREDIV)];
            if ((cfgr_reg & RCC_CFGR_PLLSRC) != RCC_CFGR_PLLSRC_HSI_DIV2) {
                /* HSE used as PLL clock source : PLLCLK = HSE/PREDIV * PLLMUL */
                pllclk = (uint32_t) ((uint64_t) BSP_CLK_SRC_SPEED / (uint64_t) (prediv)) * ((uint64_t) pllmul);
            } else {
                /* HSI used as PLL clock source : PLLCLK = HSI/2 * PLLMUL */
                pllclk = (uint32_t) ((uint64_t) (BSP_CLK_SRC_SPEED >> 1U) * ((uint64_t) pllmul));
            }

            sysclockfreq = pllclk;
            break;
        }
        case RCC_CFGR_SWS_HSE:
        case RCC_CFGR_SWS_HSI:
        default: {
            sysclockfreq = BSP_CLK_SRC_SPEED;
            break;
        }
    }
    return sysclockfreq;
}


/**
 * Configures RCC clock structure to match the desired configured freq.
 * Resulting frequency is hardcoded to be 64MHz for HCLK and APB2 Bus. APB1 bus is configured to be 32MHz
 *
 */
static void _config_rcc_clocks(void) {

    // If PLL is actually the main CLK source we cannot pre-disable it
    if ((RCC->CFGR & RCC_CFGR_SWS) != RCC_CFGR_SWS_PLL) {
        // We will change to PLL CLK source. Disable just before configure
        RCC->CR &= ~(RCC_CR_PLLON);
        while (RCC->CR & RCC_CR_PLLRDY);
    }

    // Configure the PLL to use HSI/2 as CLK base and just multiply CLK to reach 64MHz
    RCC->CFGR &= ~(RCC_CFGR_PLLMUL | RCC_CFGR_PLLSRC);
    RCC->CFGR |= (RCC_CFGR_PLLSRC_HSI_DIV2 | RCC_CFGR_PLLMUL16);

    // Enable the PLL
    RCC->CR |= RCC_CR_PLLON;
    while (!(RCC->CR & RCC_CR_PLLRDY));

    // Change FLash access latency cause we will operate at high speed
    FLASH->ACR = (FLASH->ACR & ~FLASH_ACR_LATENCY) | (FLASH_ACR_LATENCY_2);

    // Command SYSCLK source change
    RCC->CFGR = (RCC->CFGR & ~RCC_CFGR_SW) | RCC_CFGR_SW_PLL;
    // Wait to SYSCLK source effective change is committed
    while ((RCC->CFGR & RCC_CFGR_SWS) != RCC_CFGR_SWS_PLL) {}

    /* Set dividers to adjust APB1 and APB2 freqs */
    RCC->CFGR = (RCC->CFGR & ~RCC_CFGR_PPRE1) | RCC_CFGR_PPRE1_DIV2;
    RCC->CFGR = (RCC->CFGR & ~RCC_CFGR_PPRE2) | RCC_CFGR_PPRE2_DIV1;
}
