/* Copyright (C) Pablo Rodriguez Nava - All Rights Reserved
 *       * Unauthorized copying of this file, via any medium is strictly prohibited
 *       * Proprietary and confidential
 * Written by Pablo Rodriguez Nava <info@pablintino.com>, June 2021
 */

#include "includes/bsp_common_utils.h"
#include "includes/bsp_tick.h"

ret_status butil_wait_flag_status_now(const volatile uint32_t *reg, uint32_t mask, uint32_t masked_value, uint32_t timeout)
{

    return butil_wait_flag_status(reg, mask, masked_value, btick_get_ticks(), timeout);
}

ret_status butil_wait_flag_status(
    const volatile uint32_t *reg, uint32_t mask, uint32_t masked_value, uint32_t init_tick, uint32_t timeout)
{
    while (((*reg) & mask) != masked_value) {
        if (btick_get_ticks() - init_tick > timeout) {
            return STATUS_TMT;
        }
    }
    return STATUS_OK;
}
