/* Copyright (C) Pablo Rodriguez Nava - All Rights Reserved
 *       * Unauthorized copying of this file, via any medium is strictly prohibited
 *       * Proprietary and confidential
 * Written by Pablo Rodriguez Nava <info@pablintino.com>, October 2021
 */

#include "bsp_tick.h"
#include "bsp_os.h"

#include "stm32g4xx.h"

#ifdef BSP_NO_OS
volatile uint64_t bsp_tick_count = 0;
void btick_increment(void)
{
    bsp_tick_count++;
}
#endif

uint32_t btick_get_ticks(void)
{
#ifdef BSP_NO_OS
    return bsp_tick_count;
#else
    return BOS_GET_TICKS();
#endif
}

void btick_delay(uint32_t delay)
{
    uint32_t tickstart = btick_get_ticks();
    uint32_t wait = delay;

    /* Add a freq to guarantee minimum wait */
    if (wait < 0xFFFFFFFFU) {
        wait += (uint32_t)(1);
    }

    while ((btick_get_ticks() - tickstart) < wait) {
    }
}
ret_status btick_config(uint32_t sys_frequency)
{
    if (sys_frequency <= 0) {
        return STATUS_ERR;
    }
    SysTick_Config(sys_frequency / (uint32_t)BSP_SYSTICK_RATE);
    return STATUS_OK;
}
