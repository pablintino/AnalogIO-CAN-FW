/* Copyright (C) Pablo Rodriguez Nava - All Rights Reserved
 *       * Unauthorized copying of this file, via any medium is strictly prohibited
 *       * Proprietary and confidential
 * Written by Pablo Rodriguez Nava <info@pablintino.com>, June 2021
 */


#ifndef BSP_IRQ_MANAGER_H
#define BSP_IRQ_MANAGER_H


#include <cpu.h>
#include <stdbool.h>
#include "os.h"
#include "bsp_types.h"
#include "stm32g4xx.h"

#define __BSP_IRQ_IS_IRQ_ID_VALID(IRQ) ((IRQ) < 0 || (IRQ) >= MCU_IRQ_VECTOR_SIZE)

void BSP_IRQ_init(void);

ret_status BSP_IRQ_set_handler(IRQn_Type irq_id, CPU_FNCT_VOID isr);

ret_status BSP_IRQ_enable_irq(IRQn_Type irq_id);

ret_status BSP_IRQ_disable_irq(IRQn_Type irq_id);

ret_status BSP_IRQ_is_enabled(IRQn_Type irq_id, bool *status);

#endif //BSP_IRQ_MANAGER_H
