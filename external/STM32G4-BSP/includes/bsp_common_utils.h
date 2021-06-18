/* Copyright (C) Pablo Rodriguez Nava - All Rights Reserved
 *       * Unauthorized copying of this file, via any medium is strictly prohibited
 *       * Proprietary and confidential
 * Written by Pablo Rodriguez Nava <info@pablintino.com>, June 2021
 */

#ifndef BSP_COMMON_UTILS_H
#define BSP_COMMON_UTILS_H

#include <common/common_types.h>
#include "bsp_types.h"

    #define BSP_UTL_COUNT_OF(x) ((sizeof(x)/sizeof(0[x])) / ((size_t)(!(sizeof(x) % sizeof(0[x])))))

    #define __BSP_SET_MASKED_REG_VALUE(REG, MASK, VALUE) (REG) = (((REG) & ~(MASK)) | (VALUE))
    #define __BSP_SET_REG_VALUE(REG, VALUE) (REG) = (VALUE)
    #define __BSP_CLEAR_MASKED_REG(REG, MASK) REG &= (~MASK)
    #define __BSP_SET_MASKED_REG(REG, MASK) REG |= (MASK)
    #define __BSP_IS_FLAG_SET(REG, FLAG) (((REG) & (FLAG)) == FLAG)

    #define __BSP_BIT_ADDR_OFF_32(BASE, BIT) ((BASE << 5) + BIT)
    #define __BSP_BIT_ADDR_OFF_TO_BASE_POINTER_32(BASE, BITOFF) __REG32_T(BASE + ((BITOFF) >> 5))
    #define __BSP_BIT_ADDR_OFFS_TO_BIT_32(ADDR32) (1 << ((ADDR32) & 0x1f))


    ret_status BSP_UTIL_wait_flag_status_now(volatile uint32_t *reg, uint32_t mask, uint32_t masked_value, uint32_t timeout);
    ret_status BSP_UTIL_wait_flag_status(volatile uint32_t *reg, uint32_t mask, uint32_t masked_value, uint32_t init_tick, uint32_t timeout);


#endif // COMMON_UTILS_H
