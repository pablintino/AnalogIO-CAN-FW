/* Copyright (C) Pablo Rodriguez Nava - All Rights Reserved
 *       * Unauthorized copying of this file, via any medium is strictly prohibited
 *       * Proprietary and confidential
 * Written by Pablo Rodriguez Nava <info@pablintino.com>, June 2021
 */

#ifndef BSP_TICK_H
#define BSP_TICK_H


#include "bsp_types.h"

/* SysTick overflow rate set to 1 ms */
#define BSP_SYSTICK_RATE 1000U

#ifdef BSP_NO_OS
void btick_increment(void);
void btick_delay(uint32_t delay);
#endif

uint32_t btick_get_ticks(void);
ret_status btick_config(uint32_t sys_frequency);

#endif //BSP_TICK_H
