#ifndef BSP_TICK_H
#define BSP_TICK_H


#include "bsp_types.h"

#define BSP_SYSTICK_RATE 1000U

uint32_t BSP_TICK_get_ticks(void);
void BSP_TICK_config(uint32_t sys_frequency);

#endif //BSP_TICK_H
