/* Copyright (C) Pablo Rodriguez Nava - All Rights Reserved
 *       * Unauthorized copying of this file, via any medium is strictly prohibited
 *       * Proprietary and confidential
 * Written by Pablo Rodriguez Nava <info@pablintino.com>, June 2021
 */

#include "bsp_irq_manager.h"

#include <stdbool.h>

#define MCU_IRQ_VECTOR_SIZE 102

static bsp_cmn_void_cb __birq_handler_table[MCU_IRQ_VECTOR_SIZE];

static void __birq_global_irq_handler(birq_irq_id int_id);

static void __birq_default_irq_handler(void);

static inline bool __birq_is_irq_valid(birq_irq_id irq);

void BSP_IntHandlerWWDG(void)
{
    __birq_global_irq_handler(WWDG_IRQn);
}

void BSP_IntHandlerPVD(void)
{
    __birq_global_irq_handler(PVD_PVM_IRQn);
}

void BSP_IntHandlerTMP_STMP(void)
{
    __birq_global_irq_handler(RTC_TAMP_LSECSS_IRQn);
}

void BSP_IntHandlerRTC_WKUP(void)
{
    __birq_global_irq_handler(RTC_WKUP_IRQn);
}

void BSP_IntHandlerFLASH(void)
{
    __birq_global_irq_handler(FLASH_IRQn);
}

void BSP_IntHandlerRCC(void)
{
    __birq_global_irq_handler(RCC_IRQn);
}

void BSP_IntHandlerEXTI0(void)
{
    __birq_global_irq_handler(EXTI0_IRQn);
}

void BSP_IntHandlerEXTI1(void)
{
    __birq_global_irq_handler(EXTI1_IRQn);
}

void BSP_IntHandlerEXTI2(void)
{
    __birq_global_irq_handler(EXTI2_IRQn);
}

void BSP_IntHandlerEXTI3(void)
{
    __birq_global_irq_handler(EXTI3_IRQn);
}

void BSP_IntHandlerEXTI4(void)
{
    __birq_global_irq_handler(EXTI4_IRQn);
}

void BSP_IntHandlerDMA1_CH1(void)
{
    __birq_global_irq_handler(DMA1_Channel1_IRQn);
}

void BSP_IntHandlerDMA1_CH2(void)
{
    __birq_global_irq_handler(DMA1_Channel2_IRQn);
}

void BSP_IntHandlerDMA1_CH3(void)
{
    __birq_global_irq_handler(DMA1_Channel3_IRQn);
}

void BSP_IntHandlerDMA1_CH4(void)
{
    __birq_global_irq_handler(DMA1_Channel4_IRQn);
}

void BSP_IntHandlerDMA1_CH5(void)
{
    __birq_global_irq_handler(DMA1_Channel5_IRQn);
}

void BSP_IntHandlerDMA1_CH6(void)
{
    __birq_global_irq_handler(DMA1_Channel6_IRQn);
}

void BSP_IntHandlerADC1_2(void)
{
    __birq_global_irq_handler(ADC1_2_IRQn);
}

void BSP_IntHandlerUSB_HP(void)
{
    __birq_global_irq_handler(USB_HP_IRQn);
}

void BSP_IntHandlerUSB_LP(void)
{
    __birq_global_irq_handler(USB_LP_IRQn);
}

void BSP_IntHandlerFDCAN1_IT0(void)
{
    __birq_global_irq_handler(FDCAN1_IT0_IRQn);
}

void BSP_IntHandlerFDCAN1_IT1(void)
{
    __birq_global_irq_handler(FDCAN1_IT1_IRQn);
}

void BSP_IntHandlerEXTI9_5(void)
{
    __birq_global_irq_handler(EXTI9_5_IRQn);
}

void BSP_IntHandlerTIM1_BRK_TIM15(void)
{
    __birq_global_irq_handler(TIM1_BRK_TIM15_IRQn);
}

void BSP_IntHandlerTIM1_UP_TIM16(void)
{
    __birq_global_irq_handler(TIM1_UP_TIM16_IRQn);
}

void BSP_IntHandlerTIM1_TRG_COM_TIM17(void)
{
    __birq_global_irq_handler(TIM1_TRG_COM_TIM17_IRQn);
}

void BSP_IntHandlerTIM1_CC(void)
{
    __birq_global_irq_handler(TIM1_CC_IRQn);
}

void BSP_IntHandlerTIM2(void)
{
    __birq_global_irq_handler(TIM2_IRQn);
}

void BSP_IntHandlerTIM3(void)
{
    __birq_global_irq_handler(TIM3_IRQn);
}

void BSP_IntHandlerTIM4(void)
{
    __birq_global_irq_handler(TIM4_IRQn);
}

void BSP_IntHandlerI2C1_EV(void)
{
    __birq_global_irq_handler(I2C1_EV_IRQn);
}

void BSP_IntHandlerI2C1_ER(void)
{
    __birq_global_irq_handler(I2C1_ER_IRQn);
}

void BSP_IntHandlerI2C2_EV(void)
{
    __birq_global_irq_handler(I2C2_EV_IRQn);
}

void BSP_IntHandlerI2C2_ER(void)
{
    __birq_global_irq_handler(I2C2_ER_IRQn);
}

void BSP_IntHandlerSPI1(void)
{
    __birq_global_irq_handler(SPI1_IRQn);
}

void BSP_IntHandlerSPI2(void)
{
    __birq_global_irq_handler(SPI2_IRQn);
}

void BSP_IntHandlerUSART1(void)
{
    __birq_global_irq_handler(USART1_IRQn);
}

void BSP_IntHandlerUSART2(void)
{
    __birq_global_irq_handler(USART2_IRQn);
}

void BSP_IntHandlerUSART3(void)
{
    __birq_global_irq_handler(USART3_IRQn);
}

void BSP_IntHandlerEXTI15_10(void)
{
    __birq_global_irq_handler(EXTI15_10_IRQn);
}

void BSP_IntHandlerRTC_ALARM(void)
{
    __birq_global_irq_handler(RTC_Alarm_IRQn);
}

void BSP_IntHandlerUSB_WKUP(void)
{
    __birq_global_irq_handler(USBWakeUp_IRQn);
}

void BSP_IntHandlerTIM8_BRK(void)
{
    __birq_global_irq_handler(TIM8_BRK_IRQn);
}

void BSP_IntHandlerTIM8_UP(void)
{
    __birq_global_irq_handler(TIM8_UP_IRQn);
}

void BSP_IntHandlerTIM8_TRG_COM(void)
{
    __birq_global_irq_handler(TIM8_TRG_COM_IRQn);
}

void BSP_IntHandlerTIM8_CC(void)
{
    __birq_global_irq_handler(TIM8_CC_IRQn);
}

void BSP_IntHandlerLPTIM1(void)
{
    __birq_global_irq_handler(LPTIM1_IRQn);
}

void BSP_IntHandlerSPI3(void)
{
    __birq_global_irq_handler(SPI3_IRQn);
}

void BSP_IntHandlerTIM6_DAC1(void)
{
    __birq_global_irq_handler(TIM6_DAC_IRQn);
}

void BSP_IntHandlerDMA2_CH1(void)
{
    __birq_global_irq_handler(DMA2_Channel1_IRQn);
}

void BSP_IntHandlerDMA2_CH2(void)
{
    __birq_global_irq_handler(DMA2_Channel2_IRQn);
}

void BSP_IntHandlerDMA2_CH3(void)
{
    __birq_global_irq_handler(DMA2_Channel3_IRQn);
}

void BSP_IntHandlerDMA2_CH4(void)
{
    __birq_global_irq_handler(DMA2_Channel4_IRQn);
}

void BSP_IntHandlerDMA2_CH5(void)
{
    __birq_global_irq_handler(DMA2_Channel5_IRQn);
}

void BSP_IntHandlerUCPD1(void)
{
    __birq_global_irq_handler(UCPD1_IRQn);
}

void BSP_IntHandlerCOMP1_3(void)
{
    __birq_global_irq_handler(COMP1_2_3_IRQn);
}

void BSP_IntHandlerCRS(void)
{
    __birq_global_irq_handler(CRS_IRQn);
}

void BSP_IntHandlerSAI(void)
{
    __birq_global_irq_handler(SAI1_IRQn);
}

void BSP_IntHandlerFPU(void)
{
    __birq_global_irq_handler(FPU_IRQn);
}

void BSP_IntHandlerRNG(void)
{
    __birq_global_irq_handler(RNG_IRQn);
}

void BSP_IntHandlerLPUART1(void)
{
    __birq_global_irq_handler(LPUART1_IRQn);
}

void BSP_IntHandlerI2C3_EV(void)
{
    __birq_global_irq_handler(I2C3_EV_IRQn);
}

void BSP_IntHandlerI2C3_ER(void)
{
    __birq_global_irq_handler(I2C3_ER_IRQn);
}

void BSP_IntHandlerDMAMUX_OVR(void)
{
    __birq_global_irq_handler(DMAMUX_OVR_IRQn);
}

void BSP_IntHandlerDMA2_CH6(void)
{
    __birq_global_irq_handler(DMA2_Channel6_IRQn);
}

void BSP_IntHandlerCORDIC(void)
{
    __birq_global_irq_handler(CORDIC_IRQn);
}

void BSP_IntHandlerFMAC(void)
{
    __birq_global_irq_handler(FMAC_IRQn);
}

#if defined(STM32GBK1CB) || defined(STM32GK91xx) || defined(STM32G471xx) || defined(STM32G441xx) ||                    \
    defined(STM32G431xx) || defined(STM32G4A1xx)

void BSP_IntHandlerTIM7_DAC2(void)
{
    __birq_global_irq_handler(TIM7_IRQn);
}

void BSP_IntHandlerCOMP4(void)
{
    __birq_global_irq_handler(COMP4_IRQn);
}

#endif

#if defined(STM32G491xx) || defined(STM32G484xx) || defined(STM32G483xx) || defined(STM32G474xx) ||                    \
    defined(STM32G473xx) || defined(STM32G471xx) || defined(STM32G441xx) || defined(STM32G431xx) ||                    \
    defined(STM32G4A1xx)

void BSP_IntHandlerUART4(void)
{
    __birq_global_irq_handler(UART4_IRQn);
}

#endif

#if defined(STM32G484xx) || defined(STM32G483xx) || defined(STM32G474xx) || defined(STM32G473xx) || defined(STM32G471xx)

void BSP_IntHandlerI2C4_EV(void)
{
    __birq_global_irq_handler(I2C4_EV_IRQn);
}
void BSP_IntHandlerI2C4_ER(void)
{
    __birq_global_irq_handler(I2C4_ER_IRQn);
}
void BSP_IntHandlerSPI4(void)
{
    __birq_global_irq_handler(SPI4_IRQn);
}
void BSP_IntHandlerTIM5(void)
{
    __birq_global_irq_handler(TIM5_IRQn);
}

#endif

#if defined(STM32G484xx) || defined(STM32G483xx) || defined(STM32G474xx) || defined(STM32G473xx)

void BSP_IntHandlerADC4(void)
{
    __birq_global_irq_handler(ADC4_IRQn);
}
void BSP_IntHandlerADC5(void)
{
    __birq_global_irq_handler(ADC5_IRQn);
}
void BSP_IntHandlerFDCAN3_IT0(void)
{
    __birq_global_irq_handler(FDCAN3_IT0_IRQn);
}
void BSP_IntHandlerFDCAN3_IT1(void)
{
    __birq_global_irq_handler(FDCAN3_IT1_IRQn);
}
void BSP_IntHandlerFSMC(void)
{
    __birq_global_irq_handler(FMC_IRQn);
}
void BSP_IntHandlerCOMP7(void)
{
    __birq_global_irq_handler(COMP7_IRQn);
}

#endif

#if defined(STM32G484xx) || defined(STM32G474xx)

void BSP_IntHandlerHRTIM_MASTER_IRQn(void)
{
    __birq_global_irq_handler(HRTIM1_Master_IRQn);
}
void BSP_IntHandlerHRTIM_TIMA_IRQn(void)
{
    __birq_global_irq_handler(HRTIM1_TIMA_IRQn);
}
void BSP_IntHandlerHRTIM_TIMB_IRQn(void)
{
    __birq_global_irq_handler(HRTIM1_TIMB_IRQn);
}
void BSP_IntHandlerHRTIM_TIMC_IRQn(void)
{
    __birq_global_irq_handler(HRTIM1_TIMC_IRQn);
}
void BSP_IntHandlerHRTIM_TIMD_IRQn(void)
{
    __birq_global_irq_handler(HRTIM1_TIMD_IRQn);
}
void BSP_IntHandlerHRTIM_TIME_IRQn(void)
{
    __birq_global_irq_handler(HRTIM1_TIME_IRQn);
}
void BSP_IntHandlerHRTIM_FLT_IRQn(void)
{
    __birq_global_irq_handler(HRTIM1_FLT_IRQn);
}
void BSP_IntHandlerHRTIM_TIMF_IRQn(void)
{
    __birq_global_irq_handler(HRTIM1_TIMF_IRQn);
}

#endif

#if defined(STM32G484xx) || defined(STM32G483xx) || defined(STM32G441xx) || defined(STM32G4A1xx)

void BSP_IntHandlerAES(void)
{
    interrupt_handler(AES_IRQn);
}

#endif

#if defined(STM32G491xx) || defined(STM32G484xx) || defined(STM32G483xx) || defined(STM32G474xx) ||                    \
    defined(STM32G473xx) || defined(STM32G471xx) || defined(STM32G4A1xx)

void BSP_IntHandlerFDCAN2_IT0(void)
{
    __birq_global_irq_handler(FDCAN2_IT0_IRQn);
}
void BSP_IntHandlerFDCAN2_IT1(void)
{
    __birq_global_irq_handler(FDCAN2_IT1_IRQn);
}
void BSP_IntHandlerDMA2_CH7(void)
{
    __birq_global_irq_handler(DMA2_Channel7_IRQn);
}
void BSP_IntHandlerDMA2_CH8(void)
{
    __birq_global_irq_handler(DMA2_Channel8_IRQn);
}
void BSP_IntHandlerDMA1_CH7(void)
{
    __birq_global_irq_handler(DMA1_Channel7_IRQn);
}
void BSP_IntHandlerDMA1_CH8(void)
{
    __birq_global_irq_handler(DMA1_Channel8_IRQn);
}
void BSP_IntHandlerUART5(void)
{
    __birq_global_irq_handler(UART5_IRQn);
}

#endif
#if defined(STM32G491xx) || defined(STM32G484xx) || defined(STM32G483xx) || defined(STM32G474xx) ||                    \
    defined(STM32G473xx) || defined(STM32G4A1xx)

void BSP_IntHandlerQUADSPI(void)
{
    __birq_global_irq_handler(QUADSPI_IRQn);
}
void BSP_IntHandlerTIM20_BRK(void)
{
    __birq_global_irq_handler(TIM20_BRK_IRQn);
}
void BSP_IntHandlerTIM20_UP(void)
{
    __birq_global_irq_handler(TIM20_UP_IRQn);
}
void BSP_IntHandlerTIM20_TRG_COM(void)
{
    __birq_global_irq_handler(TIM20_TRG_COM_IRQn);
}
void BSP_IntHandlerTIM20_CC(void)
{
    __birq_global_irq_handler(TIM20_CC_IRQn);
}

#endif

void birq_init(void)
{
    for (uint8_t int_id = 0; int_id < MCU_IRQ_VECTOR_SIZE; int_id++) {
        birq_set_handler(int_id, __birq_default_irq_handler);
    }
}

ret_status birq_set_handler(birq_irq_id irq_id, bsp_cmn_void_cb handler)
{
    if (!__birq_is_irq_valid(irq_id)) {
        return STATUS_ERR;
    }

    BOS_CRITICAL_SECTION_BEGIN();
    __birq_handler_table[irq_id] = handler;
    BOS_CRITICAL_SECTION_EXIT();
    return STATUS_OK;
}

ret_status birq_enable_irq_with_priority(birq_irq_id irq_id, uint32_t priority, uint32_t sub_priority)
{
    if (!__birq_is_irq_valid(irq_id)) {
        return STATUS_ERR;
    }

    NVIC_SetPriority(irq_id, NVIC_EncodePriority(NVIC_GetPriorityGrouping(), priority, sub_priority));
    NVIC_EnableIRQ(irq_id);
    return STATUS_OK;
}

ret_status birq_enable_irq(birq_irq_id irq_id)
{
    if (!__birq_is_irq_valid(irq_id)) {
        return STATUS_ERR;
    }

    NVIC_EnableIRQ(irq_id);
    return STATUS_OK;
}

ret_status birq_is_enabled(birq_irq_id irq_id, bool *status)
{
    if (irq_id >= 0) {
        *status = NVIC_GetEnableIRQ(irq_id) != 0;
        return STATUS_OK;
    }
    return STATUS_ERR;
}

ret_status birq_disable_irq(birq_irq_id irq_id)
{
    if (!__birq_is_irq_valid(irq_id)) {
        return STATUS_ERR;
    }

    NVIC_DisableIRQ(irq_id);
    return STATUS_OK;
}

static inline bool __birq_is_irq_valid(birq_irq_id irq)
{
    return irq >= 0 && irq < MCU_IRQ_VECTOR_SIZE;
}

static void __birq_global_irq_handler(birq_irq_id int_id)
{
    BOS_ISR_ENTER();
    bsp_cmn_void_cb isr = __birq_handler_table[int_id];
    if (isr != (bsp_cmn_void_cb)0) {
        isr();
    }
    BOS_ISR_EXIT();
}

/**
 * Default interruption handler for all non initialized interrupts. Interrupt handlers should be declared by using
 * the birq_set_handler(CPU_DATA, CPU_FNCT_VOID) function previously.
 */
static void __birq_default_irq_handler(void)
{
    /* DO NOTHING */
}