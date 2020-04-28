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


#include  <cpu.h>
#include  "os.h"

#ifndef BP_IRQ_MANAGER_H
#define BSP_IRQ_MANAGER_H


/*
 * IRQ Table for STM32F303K8
 *
 */
#define  BSP_INT_POSITION_WWDG                  0u   /* Window WatchDog interrupt                               */
#define  BSP_INT_POSITION_PVD                   1u   /* PVD through EXTI line 16 detection interrupt            */
#define  BSP_INT_POSITION_TAMPER_STAMP          2u   /* Tamper & TimeStamp interrupts through the EXTI line 19  */
#define  BSP_INT_POSITION_RTC_WKUP              3u   /* RTC wakeup interrupt through the EXTI line 20           */
#define  BSP_INT_POSITION_FLASH                 4u   /* FLASH global interrupt                                  */
#define  BSP_INT_POSITION_RCC                   5u   /* RCC global interrupt                                    */
#define  BSP_INT_POSITION_EXTI0                 6u   /* EXTI Line0 interrupt                                    */
#define  BSP_INT_POSITION_EXTI1                 7u   /* EXTI Line1 interrupt                                    */
#define  BSP_INT_POSITION_EXTI2_TS              8u   /* EXTI Line2 & Touch sensing interrupts                   */
#define  BSP_INT_POSITION_EXTI3                 9u   /* EXTI Line3 interrupt                                    */
#define  BSP_INT_POSITION_EXTI4                10u   /* EXTI Line4 interrupt                                    */
#define  BSP_INT_POSITION_DMA1_CH1             11u   /* DMA1 Channel 1 interrupt                                */
#define  BSP_INT_POSITION_DMA1_CH2             12u   /* DMA1 Channel 2 interrupt                                */
#define  BSP_INT_POSITION_DMA1_CH3             13u   /* DMA1 Channel 3 interrupt                                */
#define  BSP_INT_POSITION_DMA1_CH4             14u   /* DMA1 Channel 4 interrupt                                */
#define  BSP_INT_POSITION_DMA1_CH5             15u   /* DMA1 Channel 5 interrupt                                */
#define  BSP_INT_POSITION_DMA1_CH6             16u   /* DMA1 Channel 6 interrupt                                */
#define  BSP_INT_POSITION_DMA1_CH7             17u   /* DMA1 Channel 7 interrupt                                */
#define  BSP_INT_POSITION_ADC1_2               18u   /* ADC1 & ADC2 global interrupt                            */
#define  BSP_INT_POSITION_CAN_TX               19u   /* CAN_TX interrupts                                       */
#define  BSP_INT_POSITION_CAN_RX0              20u   /* CAN_RX0 interrupts                                      */
#define  BSP_INT_POSITION_CAN_RX1              21u   /* CAN_TX1 interrupt                                       */
#define  BSP_INT_POSITION_CAN_SCE              22u   /* CAN_SCE interrupt                                       */
#define  BSP_INT_POSITION_EXTI9_5              23u   /* External Line[9:5] interrupts                           */
#define  BSP_INT_POSITION_TIM1_BRK_TIM15       24u   /* TIM1 Break/TIM15 global interrupts                      */
#define  BSP_INT_POSITION_TIM1_UP_TIM16        25u   /* TIM1 Update/TIM10 global interrupts                     */
#define  BSP_INT_POSITION_TIM1_TRG_COM_TIM17   26u   /* TIM1 trigger & commutation/TIM17 interrupts             */
#define  BSP_INT_POSITION_TIM1_CC              27u   /* TIM1 capture compare interrupt                          */
#define  BSP_INT_POSITION_TIM2                 28u   /* TIM2 global interrupt                                   */
#define  BSP_INT_POSITION_TIM3                 29u   /* TIM3 global interrupt                                   */
#define  BSP_INT_POSITION_I2C1_EV              31u   /* I2C1 event interrupt & EXTI Line23 interrupt            */
#define  BSP_INT_POSITION_I2C1_ER              32u   /* I2C1 Error interrupt                                    */
#define  BSP_INT_POSITION_SPI1                 35u   /* SPI1 global interrupt                                   */
#define  BSP_INT_POSITION_USART1               37u   /* USART1 global interrupt & EXTI LINE 25                  */
#define  BSP_INT_POSITION_USART2               38u   /* USART2 global interrupt & EXTI LINE 26                  */
#define  BSP_INT_POSITION_USART3               39u   /* USART3 global interrupt & EXTI LINE 28                  */
#define  BSP_INT_POSITION_EXTI15_10            40u   /* EXTI Line[15:10] interrupts                             */
#define  BSP_INT_POSITION_RTC_ALARM            41u   /* RTC alarms interrupt                                    */
#define  BSP_INT_POSITION_TIM6_DAC1            54u   /* TIM6 global & DAC1 underrun interrupts                  */
#define  BSP_INT_POSITION_TIM7_DAC2            55u   /* TIM7 global & DAC2 underrun interrupts                  */
#define  BSP_INT_POSITION_COMP2                64u   /* COMP2 combined with EXTI Lines 22 interrupt.            */
#define  BSP_INT_POSITION_COMP4_6              65u   /* COMP4 & COMP6 combined with EXTI 30 & 32 interrupts.    */
#define  BSP_INT_POSITION_FPU                  81u   /* Floating point interrupt                                */


void BSP_interrupts_init(void);

void BSP_interrupt_set_handler(CPU_DATA int_id, CPU_FNCT_VOID isr);

/*
 * IRQ handlers headers
 *
 */
void BSP_IntHandlerWWDG(void);

void BSP_IntHandlerPVD(void);

void BSP_IntHandlerTamper_STAMP(void);

void BSP_IntHandlerRTC_WKUP(void);

void BSP_IntHandlerFLASH(void);

void BSP_IntHandlerRCC(void);

void BSP_IntHandlerEXTI0(void);

void BSP_IntHandlerEXTI1(void);

void BSP_IntHandlerEXTI2_TS(void);

void BSP_IntHandlerEXTI3(void);

void BSP_IntHandlerEXTI4(void);

void BSP_IntHandlerDMA1_CH1(void);

void BSP_IntHandlerDMA1_CH2(void);

void BSP_IntHandlerDMA1_CH3(void);

void BSP_IntHandlerDMA1_CH4(void);

void BSP_IntHandlerDMA1_CH5(void);

void BSP_IntHandlerDMA1_CH6(void);

void BSP_IntHandlerDMA1_CH7(void);

void BSP_IntHandlerADC1_2(void);

void BSP_IntHandlerCAN_TX(void);

void BSP_IntHandlerCAN_RX0(void);

void BSP_IntHandlerCAN_RX1(void);

void BSP_IntHandlerCAN_SCE(void);

void BSP_IntHandlerEXTI9_5(void);

void BSP_IntHandlerTIM1_BRK_TIM15(void);

void BSP_IntHandlerTIM1_UP_TIM16(void);

void BSP_IntHandlerTIM1_TRG_COM_TIM17(void);

void BSP_IntHandlerTIM1_CC(void);

void BSP_IntHandlerTIM2(void);

void BSP_IntHandlerTIM3(void);

void BSP_IntHandlerI2C1_EV(void);

void BSP_IntHandlerI2C1_ER(void);

void BSP_IntHandlerSPI1(void);

void BSP_IntHandlerUSART1(void);

void BSP_IntHandlerUSART2(void);

void BSP_IntHandlerUSART3(void);

void BSP_IntHandlerEXTI15_10(void);

void BSP_IntHandlerRTC_Alarm(void);

void BSP_IntHandlerTIM6_DAC1(void);

void BSP_IntHandlerTIM7_DAC2(void);

void BSP_IntHandlerCOMP2(void);

void BSP_IntHandlerCOMP4_6(void);

void BSP_IntHandlerFPU(void);


#endif //BSP_IRQ_MANAGER_H
