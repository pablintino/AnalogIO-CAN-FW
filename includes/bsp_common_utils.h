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


#ifndef BSP_COMMON_UTILS_H
#define BSP_COMMON_UTILS_H

    #include <common/common_types.h>
    #include "bsp_types.h"

    #define BSP_UTL_COUNT_OF(x) ((sizeof(x)/sizeof(0[x])) / ((size_t)(!(sizeof(x) % sizeof(0[x])))))

    #define __BSP_SET_MASKED_REG_VALUE(REG, MASK, VALUE) (REG) = (((REG) & ~(MASK)) | (VALUE))
    #define __BSP_CLEAR_MASKED_REG(REG, MASK) REG &= (~MASK)
    #define __BSP_SET_MASKED_REG(REG, MASK) REG |= (MASK)
    #define __BSP_IS_FLAG_SET(REG, FLAG) (((REG) & (FLAG)) == FLAG)

    #define __BSP_BIT_ADDR_OFF_32(BASE, BIT) ((BASE << 5) + BIT)
    #define __BSP_BIT_ADDR_OFF_TO_BASE_POINTER_32(BASE, BITOFF) __REG32_T(BASE + ((BITOFF) >> 5))
    #define __BSP_BIT_ADDR_OFFS_TO_BIT_32(ADDR32) (1 << ((ADDR32) & 0x1f))


    ret_status BSP_UTIL_wait_flag_status_now(volatile uint32_t *reg, uint32_t mask, uint32_t masked_value, uint32_t timeout);
    ret_status BSP_UTIL_wait_flag_status(volatile uint32_t *reg, uint32_t mask, uint32_t masked_value, uint32_t init_tick, uint32_t timeout);


#endif // COMMON_UTILS_H
