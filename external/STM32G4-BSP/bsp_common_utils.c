/* Copyright (C) Pablo Rodriguez Nava - All Rights Reserved
 *       * Unauthorized copying of this file, via any medium is strictly prohibited
 *       * Proprietary and confidential
 * Written by Pablo Rodriguez Nava <info@pablintino.com>, June 2021
 */

#include "includes/bsp_common_utils.h"
#include "includes/bsp_tick.h"


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

