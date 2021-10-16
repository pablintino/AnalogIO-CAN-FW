/* Copyright (C) Pablo Rodriguez Nava - All Rights Reserved
 *       * Unauthorized copying of this file, via any medium is strictly prohibited
 *       * Proprietary and confidential
 * Written by Pablo Rodriguez Nava <info@pablintino.com>, October 2021
 */


#include <os.h>
#include "bsp_tick.h"
#include "stm32g4xx.h"


uint32_t btick_get_ticks(void){
    OS_ERR err;
    uint32_t ticks;
    // Obtain ticks from uC
    ticks = OSTimeGet(&err);
    if(err == OS_ERR_NONE){
        return ticks;
    }
    // TODO This situation should be managed
    return 0;
}

ret_status btick_config(uint32_t sys_frequency){
    if(sys_frequency <= 0){
        return STATUS_ERR;
    }
    SysTick_Config(sys_frequency / (uint32_t) BSP_SYSTICK_RATE);
    return STATUS_OK;
}
