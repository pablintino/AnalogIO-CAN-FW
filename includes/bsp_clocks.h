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

#include "stm32f3xx.h"
#include "bsp_config.h"

/* Actually supported HSE speeds **/
#define BSP_HSE_4MHZ (4000000UL)
#define BSP_HSE_8MHZ (8000000UL)
#define BSP_HSE_16MHZ (16000000UL)

/* Assumed to use 8MHz for all STM32 families... */
#define BSP_HSI_VALUE    ((uint32_t)8000000)


#if !defined(BSP_USE_HSE_VALUE)
#define BSP_CLK_SRC_SPEED BSP_HSI_VALUE
#elif BSP_USE_HSE_VALUE != BSP_HSE_4MHZ && BSP_USE_HSE_VALUE != BSP_HSE_8MHZ &&  BSP_USE_HSE_VALUE != BSP_HSE_16MHZ
    #error "BSP_USE_HSE_VALUE not currently supported by SW"
#else
#define BSP_CLK_SRC_SPEED BSP_USE_HSE_VALUE
#endif

#define BSP_SYSTICK_RATE 1000UL


void BSP_configure_clocks(void);
uint32_t BSP_get_system_freq(void);

#endif  //BSP_CLOCKS_H