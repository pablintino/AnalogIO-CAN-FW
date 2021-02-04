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


#include "bsp_common_utils.h"
#include "bsp_tick.h"


ret_status BSP_UTIL_wait_flag_status_now(volatile uint32_t *reg, uint32_t mask, uint32_t masked_value, uint32_t timeout) {

    return BSP_UTIL_wait_flag_status(reg, mask, masked_value, BSP_TICK_get_ticks(), timeout);
}


ret_status BSP_UTIL_wait_flag_status(volatile uint32_t *reg, uint32_t mask, uint32_t masked_value, uint32_t init_tick, uint32_t timeout) {
    while (((*reg) & mask) != masked_value) {
        if (BSP_TICK_get_ticks() - init_tick > timeout) {
            return STATUS_TMT;
        }
    }
    return STATUS_OK;
}

