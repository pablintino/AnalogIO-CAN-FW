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
#include "bsp.h"


void BSP_init(void){

    BSP_TICK_config(BSP_CLK_get_hclk_freq());

    bsp_clk_osc_config_t oscConfig;
    oscConfig.ClockType = BSP_CLK_CLOCK_TYPE_PLL | BSP_CLK_CLOCK_TYPE_HSE;
    oscConfig.HSEState = BSP_CLK_CLOCK_STATE_HSE_STATE_ENABLE;

    oscConfig.PLL.PLLSource = BSP_CLK_CLOCK_PLL_SRC_HSE;
    oscConfig.PLL.PLLState = BSP_CLK_CLOCK_STATE_PLL_STATE_ENABLE;
    oscConfig.PLL.PLLM = 2;
    oscConfig.PLL.PLLN = 8;
    oscConfig.PLL.PLLP = 2;
    oscConfig.PLL.PLLQ = 2;
    oscConfig.PLL.PLLR = 2;

    bsp_clk_clock_config_t clockConfig;
    clockConfig.ClockType = BSP_CLK_CLOCK_TYPE_SYSCLK;
    clockConfig.SystemClockSource = BSP_CLK_CLOCK_SOURCE_PLL;

    if(BSP_CLK_config_clocks_osc(&oscConfig) == STATUS_OK){
        if(BSP_CLK_config_clocks(&clockConfig) != STATUS_OK){
            /* TODO Notify this with status LED */
        }
    }else{
        /* TODO Notify this with status LED */
    }

    BSP_interrupts_init();

    BSP_CLK_PERIPH_ENABLE_GPIO(BSP_CLK_PERIPH_ENABLE_GPIO_A);

}

