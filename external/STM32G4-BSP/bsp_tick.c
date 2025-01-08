/* Copyright (C) Pablo Rodriguez Nava - All Rights Reserved
 *       * Unauthorized copying of this file, via any medium is strictly prohibited
 *       * Proprietary and confidential
 * Written by Pablo Rodriguez Nava <info@pablintino.com>, October 2021
 */

#include "bsp_tick.h"
#include "bsp_os.h"

#include "stm32g4xx.h"

uint32_t btick_get_ticks(void)
{
    return BOS_GET_TICKS();
}

ret_status btick_config(uint32_t sys_frequency)
{
    if (sys_frequency <= 0) {
        return STATUS_ERR;
    }
    SysTick_Config(sys_frequency / (uint32_t)BSP_SYSTICK_RATE);
    return STATUS_OK;
}
