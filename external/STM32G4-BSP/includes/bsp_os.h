/* Copyright (C) Pablo Rodriguez Nava - All Rights Reserved
 *       * Unauthorized copying of this file, via any medium is strictly
 * prohibited
 *       * Proprietary and confidential
 * Written by Pablo Rodriguez Nava <info@pablintino.com>, January 2025
 */

#ifndef BSP_OS_H
#define BSP_OS_H

#if defined(BSP_USING_OS_UCOS)
#include "cpu.h"
#include "cpu_core.h"
#include "os.h"

#define BOS_CRITICAL_SECTION_BEGIN()                                                                                   \
    do {                                                                                                               \
        CPU_SR_ALLOC();                                                                                                \
        CPU_CRITICAL_ENTER();                                                                                          \
    } while (0)
#define BOS_CRITICAL_SECTION_EXIT()                                                                                    \
    do {                                                                                                               \
        CPU_CRITICAL_EXIT();                                                                                           \
    } while (0)
#define BOS_ISR_ENTER()                                                                                                \
    do {                                                                                                               \
        CPU_SR_ALLOC();                                                                                                \
        CPU_CRITICAL_ENTER();                                                                                          \
        OSIntEnter();                                                                                                  \
        BOS_CRITICAL_SECTION_EXIT();                                                                                   \
    } while (0)
#define BOS_ISR_EXIT()                                                                                                 \
    do {                                                                                                               \
        OSIntExit();                                                                                                   \
    } while (0)
#define BOS_GET_TICKS()                                                                                                \
    ({                                                                                                                 \
        uint32_t retval;                                                                                               \
        OS_ERR err;                                                                                                    \
        retval = OSTimeGet(&err);                                                                                      \
        err == OS_ERR_NONE ? retval : 0;                                                                               \
    })
#define BOS_OS_MAX_PRIORITY 0
#endif

#if defined(BSP_USING_OS_FREERTOS)
#include "FreeRTOS.h"
#include "task.h"
#define BOS_GET_TICKS() xTaskGetTickCount()
#define BOS_CRITICAL_SECTION_BEGIN() ((void)0)
#define BOS_CRITICAL_SECTION_EXIT() ((void)0)
#define BOS_ISR_ENTER() ((void)0)
#define BOS_ISR_EXIT() ((void)0)
#define BOS_OS_MAX_PRIORITY configLIBRARY_MAX_SYSCALL_INTERRUPT_PRIORITY
#endif

#endif // BSP_OS_H
