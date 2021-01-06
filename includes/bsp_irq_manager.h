/**
 * MIT License
 *
 * Copyright (c) 2020 Pablo Rodriguez Nava, @pablintino
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 *
 **/


#ifndef BSP_IRQ_MANAGER_H
#define BSP_IRQ_MANAGER_H


#include  <cpu.h>
#include  "os.h"
#include "bsp_types.h"
#include "stm32g4xx.h"

#define __BSP_IRQ_IS_IRQ_ID_VALID(IRQ) ((IRQ) < 0 || (IRQ) >= MCU_IRQ_VECTOR_SIZE)

void BSP_IRQ_init(void);

ret_status BSP_IRQ_set_handler(IRQn_Type irq_id, CPU_FNCT_VOID isr);

ret_status BSP_IRQ_enable_irq(IRQn_Type irq_id);

ret_status BSP_IRQ_disable_irq(IRQn_Type irq_id);

#endif //BSP_IRQ_MANAGER_H
