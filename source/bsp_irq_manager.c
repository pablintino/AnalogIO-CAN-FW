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

#include "bsp_irq_manager.h"


#define  BSP_IRQ_VECTOR_SIZE                                 82


static CPU_FNCT_VOID BSP_IntVectTbl[BSP_IRQ_VECTOR_SIZE];

static void interrupt_handler(CPU_DATA int_id);

static void default_int_handler(void);

void BSP_IntHandlerWWDG(void) { interrupt_handler(BSP_INT_POSITION_WWDG); }

void BSP_IntHandlerPVD(void) { interrupt_handler(BSP_INT_POSITION_PVD); }

void BSP_IntHandlerTamper_STAMP(void) { interrupt_handler(BSP_INT_POSITION_TAMPER_STAMP); }

void BSP_IntHandlerRTC_WKUP(void) { interrupt_handler(BSP_INT_POSITION_RTC_WKUP); }

void BSP_IntHandlerFLASH(void) { interrupt_handler(BSP_INT_POSITION_FLASH); }

void BSP_IntHandlerRCC(void) { interrupt_handler(BSP_INT_POSITION_RCC); }

void BSP_IntHandlerEXTI0(void) { interrupt_handler(BSP_INT_POSITION_EXTI0); }

void BSP_IntHandlerEXTI1(void) { interrupt_handler(BSP_INT_POSITION_EXTI1); }

void BSP_IntHandlerEXTI2_TS(void) { interrupt_handler(BSP_INT_POSITION_EXTI2_TS); }

void BSP_IntHandlerEXTI3(void) { interrupt_handler(BSP_INT_POSITION_EXTI3); }

void BSP_IntHandlerEXTI4(void) { interrupt_handler(BSP_INT_POSITION_EXTI4); }

void BSP_IntHandlerDMA1_CH1(void) { interrupt_handler(BSP_INT_POSITION_DMA1_CH1); }

void BSP_IntHandlerDMA1_CH2(void) { interrupt_handler(BSP_INT_POSITION_DMA1_CH2); }

void BSP_IntHandlerDMA1_CH3(void) { interrupt_handler(BSP_INT_POSITION_DMA1_CH3); }

void BSP_IntHandlerDMA1_CH4(void) { interrupt_handler(BSP_INT_POSITION_DMA1_CH4); }

void BSP_IntHandlerDMA1_CH5(void) { interrupt_handler(BSP_INT_POSITION_DMA1_CH5); }

void BSP_IntHandlerDMA1_CH6(void) { interrupt_handler(BSP_INT_POSITION_DMA1_CH6); }

void BSP_IntHandlerDMA1_CH7(void) { interrupt_handler(BSP_INT_POSITION_DMA1_CH7); }

void BSP_IntHandlerADC1_2(void) { interrupt_handler(BSP_INT_POSITION_ADC1_2); }

void BSP_IntHandlerCAN_TX(void) { interrupt_handler(BSP_INT_POSITION_CAN_TX); }

void BSP_IntHandlerCAN_RX0(void) { interrupt_handler(BSP_INT_POSITION_CAN_RX0); }

void BSP_IntHandlerCAN_RX1(void) { interrupt_handler(BSP_INT_POSITION_CAN_RX1); }

void BSP_IntHandlerCAN_SCE(void) { interrupt_handler(BSP_INT_POSITION_CAN_SCE); }

void BSP_IntHandlerEXTI9_5(void) { interrupt_handler(BSP_INT_POSITION_EXTI9_5); }

void BSP_IntHandlerTIM1_BRK_TIM15(void) { interrupt_handler(BSP_INT_POSITION_TIM1_BRK_TIM15); }

void BSP_IntHandlerTIM1_UP_TIM16(void) { interrupt_handler(BSP_INT_POSITION_TIM1_UP_TIM16); }

void BSP_IntHandlerTIM1_TRG_COM_TIM17(void) { interrupt_handler(BSP_INT_POSITION_TIM1_TRG_COM_TIM17); }

void BSP_IntHandlerTIM1_CC(void) { interrupt_handler(BSP_INT_POSITION_TIM1_CC); }

void BSP_IntHandlerTIM2(void) { interrupt_handler(BSP_INT_POSITION_TIM2); }

void BSP_IntHandlerTIM3(void) { interrupt_handler(BSP_INT_POSITION_TIM3); }

void BSP_IntHandlerI2C1_EV(void) { interrupt_handler(BSP_INT_POSITION_I2C1_EV); }

void BSP_IntHandlerI2C1_ER(void) { interrupt_handler(BSP_INT_POSITION_I2C1_ER); }

void BSP_IntHandlerSPI1(void) { interrupt_handler(BSP_INT_POSITION_SPI1); }

void BSP_IntHandlerUSART1(void) { interrupt_handler(BSP_INT_POSITION_USART1); }

void BSP_IntHandlerUSART2(void) { interrupt_handler(BSP_INT_POSITION_USART2); }

void BSP_IntHandlerUSART3(void) { interrupt_handler(BSP_INT_POSITION_USART3); }

void BSP_IntHandlerEXTI15_10(void) { interrupt_handler(BSP_INT_POSITION_EXTI15_10); }

void BSP_IntHandlerRTC_Alarm(void) { interrupt_handler(BSP_INT_POSITION_RTC_ALARM); }

void BSP_IntHandlerTIM6_DAC1(void) { interrupt_handler(BSP_INT_POSITION_TIM6_DAC1); }

void BSP_IntHandlerTIM7_DAC2(void) { interrupt_handler(BSP_INT_POSITION_TIM7_DAC2); }

void BSP_IntHandlerCOMP2(void) { interrupt_handler(BSP_INT_POSITION_COMP2); }

void BSP_IntHandlerCOMP4_6(void) { interrupt_handler(BSP_INT_POSITION_COMP4_6); }

void BSP_IntHandlerFPU(void) { interrupt_handler(BSP_INT_POSITION_FPU); }


void BSP_interrupts_init(void) {
    for (CPU_DATA int_id = 0; int_id < BSP_IRQ_VECTOR_SIZE; int_id++) {
        BSP_interrupt_set_handler(int_id, default_int_handler);
    }
}


void BSP_interrupt_set_handler(CPU_DATA int_id, CPU_FNCT_VOID isr) {
    CPU_SR_ALLOC();

    if (int_id < BSP_IRQ_VECTOR_SIZE) {
        CPU_CRITICAL_ENTER();
        BSP_IntVectTbl[int_id] = isr;
        CPU_CRITICAL_EXIT();
    }
}

static void interrupt_handler(CPU_DATA int_id) {
    CPU_FNCT_VOID isr;
    CPU_SR_ALLOC();

    CPU_CRITICAL_ENTER();

    OSIntEnter();

    CPU_CRITICAL_EXIT();

    if (int_id < BSP_IRQ_VECTOR_SIZE) {
        isr = BSP_IntVectTbl[int_id];
        if (isr != (CPU_FNCT_VOID) 0) {
            isr();
        }
    }

    OSIntExit();
}


/**
 * Default interruption handler for all non initialized interrupts. Interrupt handlers should be declared by using
 * the BSP_interrupt_set_handler(CPU_DATA, CPU_FNCT_VOID) function previously.
 */
static void default_int_handler(void) {

}