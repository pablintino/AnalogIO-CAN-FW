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

#define  MCU_IRQ_VECTOR_SIZE                                 102

static CPU_FNCT_VOID BSP_IntVectTbl[MCU_IRQ_VECTOR_SIZE];

static void interrupt_handler(IRQn_Type int_id);

static void default_int_handler(void);

void BSP_IntHandlerWWDG(void) { interrupt_handler(WWDG_IRQn); }

void BSP_IntHandlerPVD(void) { interrupt_handler(PVD_PVM_IRQn); }

void BSP_IntHandlerTMP_STMP(void) { interrupt_handler(RTC_TAMP_LSECSS_IRQn); }

void BSP_IntHandlerRTC_WKUP(void) { interrupt_handler(RTC_WKUP_IRQn); }

void BSP_IntHandlerFLASH(void) { interrupt_handler(FLASH_IRQn); }

void BSP_IntHandlerRCC(void) { interrupt_handler(RCC_IRQn); }

void BSP_IntHandlerEXTI0(void) { interrupt_handler(EXTI0_IRQn); }

void BSP_IntHandlerEXTI1(void) { interrupt_handler(EXTI1_IRQn); }

void BSP_IntHandlerEXTI2(void) { interrupt_handler(EXTI2_IRQn); }

void BSP_IntHandlerEXTI3(void) { interrupt_handler(EXTI3_IRQn); }

void BSP_IntHandlerEXTI4(void) { interrupt_handler(EXTI4_IRQn); }

void BSP_IntHandlerDMA1_CH1(void) { interrupt_handler(DMA1_Channel1_IRQn); }

void BSP_IntHandlerDMA1_CH2(void) { interrupt_handler(DMA1_Channel2_IRQn); }

void BSP_IntHandlerDMA1_CH3(void) { interrupt_handler(DMA1_Channel3_IRQn); }

void BSP_IntHandlerDMA1_CH4(void) { interrupt_handler(DMA1_Channel4_IRQn); }

void BSP_IntHandlerDMA1_CH5(void) { interrupt_handler(DMA1_Channel5_IRQn); }

void BSP_IntHandlerDMA1_CH6(void) { interrupt_handler(DMA1_Channel6_IRQn); }

void BSP_IntHandlerADC1_2(void) { interrupt_handler(ADC1_2_IRQn); }

void BSP_IntHandlerUSB_HP(void) { interrupt_handler(USB_HP_IRQn); }

void BSP_IntHandlerUSB_LP(void) { interrupt_handler(USB_LP_IRQn); }

void BSP_IntHandlerFDCAN1_IT0(void) { interrupt_handler(FDCAN1_IT0_IRQn); }

void BSP_IntHandlerFDCAN1_IT1(void) { interrupt_handler(FDCAN1_IT1_IRQn); }

void BSP_IntHandlerEXTI9_5(void) { interrupt_handler(EXTI9_5_IRQn); }

void BSP_IntHandlerTIM1_BRK_TIM15(void) { interrupt_handler(TIM1_BRK_TIM15_IRQn); }

void BSP_IntHandlerTIM1_UP_TIM16(void) { interrupt_handler(TIM1_UP_TIM16_IRQn); }

void BSP_IntHandlerTIM1_TRG_COM_TIM17(void) { interrupt_handler(TIM1_TRG_COM_TIM17_IRQn); }

void BSP_IntHandlerTIM1_CC(void) { interrupt_handler(TIM1_CC_IRQn); }

void BSP_IntHandlerTIM2(void) { interrupt_handler(TIM2_IRQn); }

void BSP_IntHandlerTIM3(void) { interrupt_handler(TIM3_IRQn); }

void BSP_IntHandlerTIM4(void) { interrupt_handler(TIM4_IRQn); }

void BSP_IntHandlerI2C1_EV(void) { interrupt_handler(I2C1_EV_IRQn); }

void BSP_IntHandlerI2C1_ER(void) { interrupt_handler(I2C1_ER_IRQn); }

void BSP_IntHandlerI2C2_EV(void) { interrupt_handler(I2C2_EV_IRQn); }

void BSP_IntHandlerI2C2_ER(void) { interrupt_handler(I2C2_ER_IRQn); }

void BSP_IntHandlerSPI1(void) { interrupt_handler(SPI1_IRQn); }

void BSP_IntHandlerSPI2(void) { interrupt_handler(SPI2_IRQn); }

void BSP_IntHandlerUSART1(void) { interrupt_handler(USART1_IRQn); }

void BSP_IntHandlerUSART2(void) { interrupt_handler(USART2_IRQn); }

void BSP_IntHandlerUSART3(void) { interrupt_handler(USART3_IRQn); }

void BSP_IntHandlerEXTI15_10(void) { interrupt_handler(EXTI15_10_IRQn); }

void BSP_IntHandlerRTC_ALARM(void) { interrupt_handler(RTC_Alarm_IRQn); }

void BSP_IntHandlerUSB_WKUP(void) { interrupt_handler(USBWakeUp_IRQn); }

void BSP_IntHandlerTIM8_BRK(void) { interrupt_handler(TIM8_BRK_IRQn); }

void BSP_IntHandlerTIM8_UP(void) { interrupt_handler(TIM8_UP_IRQn); }

void BSP_IntHandlerTIM8_TRG_COM(void) { interrupt_handler(TIM8_TRG_COM_IRQn); }

void BSP_IntHandlerTIM8_CC(void) { interrupt_handler(TIM8_CC_IRQn); }

void BSP_IntHandlerLPTIM1(void) { interrupt_handler(LPTIM1_IRQn); }

void BSP_IntHandlerSPI3(void) { interrupt_handler(SPI3_IRQn); }

void BSP_IntHandlerTIM6_DAC1(void) { interrupt_handler(TIM6_DAC_IRQn); }

void BSP_IntHandlerDMA2_CH1(void) { interrupt_handler(DMA2_Channel1_IRQn); }

void BSP_IntHandlerDMA2_CH2(void) { interrupt_handler(DMA2_Channel2_IRQn); }

void BSP_IntHandlerDMA2_CH3(void) { interrupt_handler(DMA2_Channel3_IRQn); }

void BSP_IntHandlerDMA2_CH4(void) { interrupt_handler(DMA2_Channel4_IRQn); }

void BSP_IntHandlerDMA2_CH5(void) { interrupt_handler(DMA2_Channel5_IRQn); }

void BSP_IntHandlerUCPD1(void) { interrupt_handler(UCPD1_IRQn); }

void BSP_IntHandlerCOMP1_3(void) { interrupt_handler(COMP1_2_3_IRQn); }

void BSP_IntHandlerCRS(void) { interrupt_handler(CRS_IRQn); }

void BSP_IntHandlerSAI(void) { interrupt_handler(SAI1_IRQn); }

void BSP_IntHandlerFPU(void) { interrupt_handler(FPU_IRQn); }

void BSP_IntHandlerRNG(void) { interrupt_handler(RNG_IRQn); }

void BSP_IntHandlerLPUART1(void) { interrupt_handler(LPUART1_IRQn); }

void BSP_IntHandlerI2C3_EV(void) { interrupt_handler(I2C3_EV_IRQn); }

void BSP_IntHandlerI2C3_ER(void) { interrupt_handler(I2C3_ER_IRQn); }

void BSP_IntHandlerDMAMUX_OVR(void) { interrupt_handler(DMAMUX_OVR_IRQn); }

void BSP_IntHandlerDMA2_CH6(void) { interrupt_handler(DMA2_Channel6_IRQn); }

void BSP_IntHandlerCORDIC(void) { interrupt_handler(CORDIC_IRQn); }

void BSP_IntHandlerFMAC(void) { interrupt_handler(FMAC_IRQn); }

#if defined(STM32GBK1CB) || defined(STM32GK91xx) || defined(STM32G471xx) || defined(STM32G441xx) || defined(STM32G431xx)\
 || defined(STM32G4A1xx)

void BSP_IntHandlerTIM7_DAC2(void) { interrupt_handler(TIM7_IRQn); }

void BSP_IntHandlerCOMP4(void) { interrupt_handler(COMP4_IRQn); }

#endif

#if defined(STM32G491xx) || defined(STM32G484xx) || defined(STM32G483xx) || defined(STM32G474xx) \
 || defined(STM32G473xx) || defined(STM32G471xx) || defined(STM32G441xx) || defined(STM32G431xx) || defined(STM32G4A1xx)

void BSP_IntHandlerUART4(void) { interrupt_handler(UART4_IRQn); }

#endif

#if defined(STM32G484xx) || defined(STM32G483xx) || defined(STM32G474xx) || defined(STM32G473xx) || defined(STM32G471xx)

void BSP_IntHandlerI2C4_EV(void) { interrupt_handler(I2C4_EV_IRQn); }
void BSP_IntHandlerI2C4_ER(void) { interrupt_handler(I2C4_ER_IRQn); }
void BSP_IntHandlerSPI4(void) { interrupt_handler(SPI4_IRQn); }
void BSP_IntHandlerTIM5(void) { interrupt_handler(TIM5_IRQn); }

#endif

#if defined(STM32G484xx) || defined(STM32G483xx) || defined(STM32G474xx) || defined(STM32G473xx)

void BSP_IntHandlerADC4(void) { interrupt_handler(ADC4_IRQn); }
void BSP_IntHandlerADC5(void) { interrupt_handler(ADC5_IRQn); }
void BSP_IntHandlerFDCAN3_IT0(void) { interrupt_handler(FDCAN3_IT0_IRQn); }
void BSP_IntHandlerFDCAN3_IT1(void) { interrupt_handler(FDCAN3_IT1_IRQn); }
void BSP_IntHandlerFSMC(void) { interrupt_handler(FMC_IRQn); }
void BSP_IntHandlerCOMP7(void) { interrupt_handler(COMP7_IRQn); }

#endif

#if defined(STM32G484xx) || defined(STM32G474xx)

void BSP_IntHandlerHRTIM_MASTER_IRQn(void) { interrupt_handler(HRTIM1_Master_IRQn); }
void BSP_IntHandlerHRTIM_TIMA_IRQn(void) { interrupt_handler(HRTIM1_TIMA_IRQn); }
void BSP_IntHandlerHRTIM_TIMB_IRQn(void) { interrupt_handler(HRTIM1_TIMB_IRQn); }
void BSP_IntHandlerHRTIM_TIMC_IRQn(void) { interrupt_handler(HRTIM1_TIMC_IRQn); }
void BSP_IntHandlerHRTIM_TIMD_IRQn(void) { interrupt_handler(HRTIM1_TIMD_IRQn); }
void BSP_IntHandlerHRTIM_TIME_IRQn(void) { interrupt_handler(HRTIM1_TIME_IRQn); }
void BSP_IntHandlerHRTIM_FLT_IRQn(void) { interrupt_handler(HRTIM1_FLT_IRQn); }
void BSP_IntHandlerHRTIM_TIMF_IRQn(void) { interrupt_handler(HRTIM1_TIMF_IRQn); }

#endif

#if defined(STM32G484xx) || defined(STM32G483xx) || defined(STM32G441xx) || defined(STM32G4A1xx)

void BSP_IntHandlerAES(void) { interrupt_handler(AES_IRQn); }

#endif

#if defined(STM32G491xx) || defined(STM32G484xx) || defined(STM32G483xx) || defined(STM32G474xx) \
 || defined(STM32G473xx) || defined(STM32G471xx) || defined(STM32G4A1xx)

void BSP_IntHandlerFDCAN2_IT0(void) { interrupt_handler(FDCAN2_IT0_IRQn); }
void BSP_IntHandlerFDCAN2_IT1(void) { interrupt_handler(FDCAN2_IT1_IRQn); }
void BSP_IntHandlerDMA2_CH7(void){ interrupt_handler(DMA2_Channel7_IRQn); }
void BSP_IntHandlerDMA2_CH8(void){ interrupt_handler(DMA2_Channel8_IRQn); }
void BSP_IntHandlerDMA1_CH7(void) { interrupt_handler(DMA1_Channel7_IRQn); }
void BSP_IntHandlerDMA1_CH8(void) { interrupt_handler(DMA1_Channel8_IRQn); }
void BSP_IntHandlerUART5(void) { interrupt_handler(UART5_IRQn); }

#endif
#if defined(STM32G491xx) || defined(STM32G484xx) || defined(STM32G483xx) || defined(STM32G474xx) \
 || defined(STM32G473xx) || defined(STM32G4A1xx)

void BSP_IntHandlerQUADSPI(void) { interrupt_handler(QUADSPI_IRQn); }
void BSP_IntHandlerTIM20_BRK(void) { interrupt_handler(TIM20_BRK_IRQn); }
void BSP_IntHandlerTIM20_UP(void) { interrupt_handler(TIM20_UP_IRQn); }
void BSP_IntHandlerTIM20_TRG_COM(void) { interrupt_handler(TIM20_TRG_COM_IRQn); }
void BSP_IntHandlerTIM20_CC(void) { interrupt_handler(TIM20_CC_IRQn); }

#endif

void BSP_IRQ_init(void) {
    for (uint8_t int_id = 0; int_id < MCU_IRQ_VECTOR_SIZE; int_id++) {
        BSP_IRQ_set_handler(int_id, default_int_handler);
    }
}

ret_status BSP_IRQ_set_handler(IRQn_Type irq_id, CPU_FNCT_VOID isr) {
    CPU_SR_ALLOC();

    if (__BSP_IRQ_IS_IRQ_ID_VALID(irq_id)) {
        return STATUS_ERR;
    }

    CPU_CRITICAL_ENTER();
    BSP_IntVectTbl[irq_id] = isr;
    CPU_CRITICAL_EXIT();
    return STATUS_OK;

}


ret_status BSP_IRQ_enable_irq(IRQn_Type irq_id) {
    if (__BSP_IRQ_IS_IRQ_ID_VALID(irq_id)) {
        return STATUS_ERR;
    }

    NVIC_EnableIRQ(irq_id);
    return STATUS_OK;
}

ret_status BSP_IRQ_disable_irq(IRQn_Type irq_id) {
    if (__BSP_IRQ_IS_IRQ_ID_VALID(irq_id)) {
        return STATUS_ERR;
    }

    NVIC_DisableIRQ(irq_id);
    return STATUS_OK;
}

static void interrupt_handler(IRQn_Type int_id) {
    CPU_FNCT_VOID isr;
    CPU_SR_ALLOC();

    CPU_CRITICAL_ENTER();

    OSIntEnter();

    CPU_CRITICAL_EXIT();

    if (int_id < MCU_IRQ_VECTOR_SIZE) {
        isr = BSP_IntVectTbl[int_id];
        if (isr != (CPU_FNCT_VOID) 0) {
            isr();
        }
    }

    OSIntExit();
}


/**
 * Default interruption handler for all non initialized interrupts. Interrupt handlers should be declared by using
 * the BSP_IRQ_set_handler(CPU_DATA, CPU_FNCT_VOID) function previously.
 */
static void default_int_handler(void) {
    /* DO NOTHING */
}