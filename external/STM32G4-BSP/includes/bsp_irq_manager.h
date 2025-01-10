/* Copyright (C) Pablo Rodriguez Nava - All Rights Reserved
 *       * Unauthorized copying of this file, via any medium is strictly prohibited
 *       * Proprietary and confidential
 * Written by Pablo Rodriguez Nava <info@pablintino.com>, June 2021
 */

#ifndef BSP_IRQ_MANAGER_H
#define BSP_IRQ_MANAGER_H

#include "bsp_os.h"
#include "bsp_types.h"
#include "common/common_types.h"

#include "stm32g4xx.h"
#include <stdbool.h>

#ifndef BSP_IRQ_MANAGER_DEFAULT_PRIORITY
#define BSP_IRQ_MANAGER_DEFAULT_PRIORITY BOS_OS_MAX_PRIORITY
#endif

#ifndef BSP_IRQ_MANAGER_DEFAULT_SUB_PRIORITY
#define BSP_IRQ_MANAGER_DEFAULT_SUB_PRIORITY (uint32_t)(0)
#endif

typedef IRQn_Type birq_irq_id;

void birq_init(void);

ret_status birq_set_handler(birq_irq_id irq_id, bsp_cmn_void_cb handler);

ret_status birq_enable_irq(birq_irq_id irq_id);

ret_status birq_enable_irq_with_priority(birq_irq_id irq_id, uint32_t priority, uint32_t sub_priority);

ret_status birq_disable_irq(birq_irq_id irq_id);

ret_status birq_is_enabled(birq_irq_id irq_id, bool *status);

#endif // BSP_IRQ_MANAGER_H
