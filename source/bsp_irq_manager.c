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



static CPU_FNCT_VOID BSP_IntVectTbl[BSP_IRQ_VECTOR_SIZE];

static void interrupt_handler(uint16_t int_id);

static void default_int_handler(void);


void BSP_IntHandlerWWDG(void){ interrupt_handler(BSP_INT_POSITION_WWDG); }
void BSP_IntHandlerPVD(void){ interrupt_handler(BSP_INT_POSITION_PVD); }
void BSP_IntHandlerTMP_STMP(void){ interrupt_handler(BSP_INT_POSITION_TAMPER_STAMP); }
void BSP_IntHandlerRTC_WKUP(void){ interrupt_handler(BSP_INT_POSITION_RTC_WKUP); }
void BSP_IntHandlerFLASH(void){ interrupt_handler(BSP_INT_POSITION_FLASH); }
void BSP_IntHandlerRCC(void){ interrupt_handler(BSP_INT_POSITION_RCC); }
void BSP_IntHandlerEXTI0(void){ interrupt_handler(BSP_INT_POSITION_EXTI0); }
void BSP_IntHandlerEXTI1(void){ interrupt_handler(BSP_INT_POSITION_EXTI1); }
void BSP_IntHandlerEXTI2(void){ interrupt_handler(BSP_INT_POSITION_EXTI2); }
void BSP_IntHandlerEXTI3(void){ interrupt_handler(BSP_INT_POSITION_EXTI3); }
void BSP_IntHandlerEXTI4(void){ interrupt_handler(BSP_INT_POSITION_EXTI4); }
void BSP_IntHandlerDMA1_CH1(void){ interrupt_handler(BSP_INT_POSITION_DMA1_CH1); }
void BSP_IntHandlerDMA1_CH2(void){ interrupt_handler(BSP_INT_POSITION_DMA1_CH2); }
void BSP_IntHandlerDMA1_CH3(void){ interrupt_handler(BSP_INT_POSITION_DMA1_CH3); }
void BSP_IntHandlerDMA1_CH4(void){ interrupt_handler(BSP_INT_POSITION_DMA1_CH4); }
void BSP_IntHandlerDMA1_CH5(void){ interrupt_handler(BSP_INT_POSITION_DMA1_CH5); }
void BSP_IntHandlerDMA1_CH6(void){ interrupt_handler(BSP_INT_POSITION_DMA1_CH6); }
void BSP_IntHandlerDMA1_CH7(void){ interrupt_handler(BSP_INT_POSITION_DMA1_CH7); }
void BSP_IntHandlerADC1_2(void){ interrupt_handler(BSP_INT_POSITION_ADC1_2); }
void BSP_IntHandlerUSB_HP(void){ interrupt_handler(BSP_INT_POSITION_USB_HP); }
void BSP_IntHandlerUSB_LP(void){ interrupt_handler(BSP_INT_POSITION_USB_LP); }
void BSP_IntHandlerFDCAN1_IT0(void){ interrupt_handler(BSP_INT_POSITION_FDCAN1_IT0); }
void BSP_IntHandlerFDCAN1_IT1(void){ interrupt_handler(BSP_INT_POSITION_FDCAN1_IT1); }
void BSP_IntHandlerEXTI9_5(void){ interrupt_handler(BSP_INT_POSITION_EXTI9_5); }
void BSP_IntHandlerTIM1_BRK_TIM15(void){ interrupt_handler(BSP_INT_POSITION_TIM1_BRK_TIM15); }
void BSP_IntHandlerTIM1_UP_TIM16(void){ interrupt_handler(BSP_INT_POSITION_TIM1_UP_TIM16); }
void BSP_IntHandlerTIM1_TRG_COM_TIM17(void){ interrupt_handler(BSP_INT_POSITION_TIM1_TRG_COM_TIM17); }
void BSP_IntHandlerTIM1_CC(void){ interrupt_handler(BSP_INT_POSITION_TIM1_CC); }
void BSP_IntHandlerTIM2(void){ interrupt_handler(BSP_INT_POSITION_TIM2); }
void BSP_IntHandlerTIM3(void){ interrupt_handler(BSP_INT_POSITION_TIM3); }
void BSP_IntHandlerTIM4(void){ interrupt_handler(BSP_INT_POSITION_TIM4); }
void BSP_IntHandlerI2C1_EV(void){ interrupt_handler(BSP_INT_POSITION_I2C1_EV); }
void BSP_IntHandlerI2C1_ER(void){ interrupt_handler(BSP_INT_POSITION_I2C1_ER); }
void BSP_IntHandlerI2C2_EV(void){ interrupt_handler(BSP_INT_POSITION_I2C2_EV); }
void BSP_IntHandlerI2C2_ER(void){ interrupt_handler(BSP_INT_POSITION_I2C2_ER); }
void BSP_IntHandlerSPI1(void){ interrupt_handler(BSP_INT_POSITION_SPI1); }
void BSP_IntHandlerSPI2(void){ interrupt_handler(BSP_INT_POSITION_SPI2); }
void BSP_IntHandlerUSART1(void){ interrupt_handler(BSP_INT_POSITION_USART1); }
void BSP_IntHandlerUSART2(void){ interrupt_handler(BSP_INT_POSITION_USART2); }
void BSP_IntHandlerUSART3(void){ interrupt_handler(BSP_INT_POSITION_USART3); }
void BSP_IntHandlerEXTI15_10(void){ interrupt_handler(BSP_INT_POSITION_EXTI15_10); }
void BSP_IntHandlerRTC_ALARM(void){ interrupt_handler(BSP_INT_POSITION_RTC_ALARM); }
void BSP_IntHandlerUSB_WKUP(void){ interrupt_handler(BSP_INT_POSITION_USB_WKUP); }
void BSP_IntHandlerTIM8_BRK(void){ interrupt_handler(BSP_INT_POSITION_TIM8_BRK); }
void BSP_IntHandlerTIM8_UP(void){ interrupt_handler(BSP_INT_POSITION_TIM8_UP); }
void BSP_IntHandlerTIM8_TRG_COM(void){ interrupt_handler(BSP_INT_POSITION_TIM8_TRG_COM); }
void BSP_IntHandlerTIM8_CC(void){ interrupt_handler(BSP_INT_POSITION_TIM8_CC); }
void BSP_IntHandlerADC3(void){ interrupt_handler(BSP_INT_POSITION_ADC3); }
void BSP_IntHandlerFSMC(void){ interrupt_handler(BSP_INT_POSITION_FSMC); }
void BSP_IntHandlerLPTIM1(void){ interrupt_handler(BSP_INT_POSITION_LPTIM1); }
void BSP_IntHandlerTIM5(void){ interrupt_handler(BSP_INT_POSITION_TIM5); }
void BSP_IntHandlerSPI3(void){ interrupt_handler(BSP_INT_POSITION_SPI3); }
void BSP_IntHandlerUART4(void){ interrupt_handler(BSP_INT_POSITION_UART4); }
void BSP_IntHandlerUART5(void){ interrupt_handler(BSP_INT_POSITION_UART5); }
void BSP_IntHandlerTIM6_DAC1(void){ interrupt_handler(BSP_INT_POSITION_TIM6_DAC1); }
void BSP_IntHandlerTIM7_DAC2(void){ interrupt_handler(BSP_INT_POSITION_TIM7_DAC2); }
void BSP_IntHandlerDMA2_CH1(void){ interrupt_handler(BSP_INT_POSITION_DMA2_CH1); }
void BSP_IntHandlerDMA2_CH2(void){ interrupt_handler(BSP_INT_POSITION_DMA2_CH2); }
void BSP_IntHandlerDMA2_CH3(void){ interrupt_handler(BSP_INT_POSITION_DMA2_CH3); }
void BSP_IntHandlerDMA2_CH4(void){ interrupt_handler(BSP_INT_POSITION_DMA2_CH4); }
void BSP_IntHandlerDMA2_CH5(void){ interrupt_handler(BSP_INT_POSITION_DMA2_CH5); }
void BSP_IntHandlerADC4(void){ interrupt_handler(BSP_INT_POSITION_ADC4); }
void BSP_IntHandlerADC5(void){ interrupt_handler(BSP_INT_POSITION_ADC5); }
void BSP_IntHandlerUCPD1(void){ interrupt_handler(BSP_INT_POSITION_UCPD1); }
void BSP_IntHandlerCOMP1_3(void){ interrupt_handler(BSP_INT_POSITION_COMP1_3); }
void BSP_IntHandlerCOMP4_6(void){ interrupt_handler(BSP_INT_POSITION_COMP4_6); }
void BSP_IntHandlerCOMP7(void){ interrupt_handler(BSP_INT_POSITION_COMP7); }
void BSP_IntHandlerHRTIM_MASTER_IRQn(void){ interrupt_handler(BSP_INT_POSITION_HRTIM_MASTER_IRQn); }
void BSP_IntHandlerHRTIM_TIMA_IRQn(void){ interrupt_handler(BSP_INT_POSITION_HRTIM_TIMA_IRQn); }
void BSP_IntHandlerHRTIM_TIMB_IRQn(void){ interrupt_handler(BSP_INT_POSITION_HRTIM_TIMB_IRQn); }
void BSP_IntHandlerHRTIM_TIMC_IRQn(void){ interrupt_handler(BSP_INT_POSITION_HRTIM_TIMC_IRQn); }
void BSP_IntHandlerHRTIM_TIMD_IRQn(void){ interrupt_handler(BSP_INT_POSITION_HRTIM_TIMD_IRQn); }
void BSP_IntHandlerHRTIM_TIME_IRQn(void){ interrupt_handler(BSP_INT_POSITION_HRTIM_TIME_IRQn); }
void BSP_IntHandlerHRTIM_FLT_IRQn(void){ interrupt_handler(BSP_INT_POSITION_HRTIM_FLT_IRQn); }
void BSP_IntHandlerHRTIM_TIMF_IRQn(void){ interrupt_handler(BSP_INT_POSITION_HRTIM_TIMF_IRQn); }
void BSP_IntHandlerCRS(void){ interrupt_handler(BSP_INT_POSITION_CRS); }
void BSP_IntHandlerSAI(void){ interrupt_handler(BSP_INT_POSITION_SAI); };
void BSP_IntHandlerTIM20_BRK(void){ interrupt_handler(BSP_INT_POSITION_TIM20_BRK); }
void BSP_IntHandlerTIM20_UP(void){ interrupt_handler(BSP_INT_POSITION_TIM20_UP); }
void BSP_IntHandlerTIM20_TRG_COM(void){ interrupt_handler(BSP_INT_POSITION_TIM20_TRG_COM); }
void BSP_IntHandlerTIM20_CC(void){ interrupt_handler(BSP_INT_POSITION_TIM20_CC); }
void BSP_IntHandlerFPU(void){ interrupt_handler(BSP_INT_POSITION_FPU); }
void BSP_IntHandlerI2C4_EV(void){ interrupt_handler(BSP_INT_POSITION_I2C4_EV); }
void BSP_IntHandlerI2C4_ER(void){ interrupt_handler(BSP_INT_POSITION_I2C4_ER); }
void BSP_IntHandlerSPI4(void){ interrupt_handler(BSP_INT_POSITION_SPI4); }
void BSP_IntHandlerAES(void){ interrupt_handler(BSP_INT_POSITION_AES); }
void BSP_IntHandlerFDCAN2_IT0(void){ interrupt_handler(BSP_INT_POSITION_FDCAN2_IT0); }
void BSP_IntHandlerFDCAN2_IT1(void){ interrupt_handler(BSP_INT_POSITION_FDCAN2_IT1); }
void BSP_IntHandlerFDCAN3_IT0(void){ interrupt_handler(BSP_INT_POSITION_FDCAN3_IT0); }
void BSP_IntHandlerFDCAN3_IT1(void){ interrupt_handler(BSP_INT_POSITION_FDCAN3_IT1); }
void BSP_IntHandlerRNG(void){ interrupt_handler(BSP_INT_POSITION_RNG); }
void BSP_IntHandlerLPUART1(void){ interrupt_handler(BSP_INT_POSITION_LPUART1); }
void BSP_IntHandlerI2C3_EV(void){ interrupt_handler(BSP_INT_POSITION_I2C3_EV); }
void BSP_IntHandlerI2C3_ER(void){ interrupt_handler(BSP_INT_POSITION_I2C3_ER); }
void BSP_IntHandlerDMAMUX_OVR(void){ interrupt_handler(BSP_INT_POSITION_DMAMUX_OVR); }
void BSP_IntHandlerQUADSPI(void){ interrupt_handler(BSP_INT_POSITION_QUADSPI); }
void BSP_IntHandlerDMA1_CH8(void){ interrupt_handler(BSP_INT_POSITION_DMA1_CH8); }
void BSP_IntHandlerDMA2_CH6(void){ interrupt_handler(BSP_INT_POSITION_DMA2_CH6); }
void BSP_IntHandlerDMA2_CH7(void){ interrupt_handler(BSP_INT_POSITION_DMA2_CH7); }
void BSP_IntHandlerDMA2_CH8(void){ interrupt_handler(BSP_INT_POSITION_DMA2_CH8); }
void BSP_IntHandlerCORDIC(void){ interrupt_handler(BSP_INT_POSITION_CORDIC); }
void BSP_IntHandlerFMAC(void){ interrupt_handler(BSP_INT_POSITION_FMAC); }



void BSP_interrupts_init(void) {
    for (uint16_t int_id = 0; int_id < BSP_IRQ_VECTOR_SIZE; int_id++) {
        BSP_interrupt_set_handler(int_id, default_int_handler);
    }
}


void BSP_interrupt_set_handler(uint16_t int_id, CPU_FNCT_VOID isr) {
    CPU_SR_ALLOC();

    if (int_id < BSP_IRQ_VECTOR_SIZE) {
        CPU_CRITICAL_ENTER();
        BSP_IntVectTbl[int_id] = isr;
        CPU_CRITICAL_EXIT();
    }
}

static void interrupt_handler(uint16_t int_id) {
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