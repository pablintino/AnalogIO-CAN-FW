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
#include "bsp_types.h"

#ifndef BP_IRQ_MANAGER_H
#define BSP_IRQ_MANAGER_H



/*
 * STM32G IRQ Table size
 *
 */
#define  BSP_IRQ_VECTOR_SIZE                                 102


/*
 * IRQ Table for STM32G431KB
 *
 */
#define  BSP_INT_POSITION_WWDG                  0u   /* Window WatchDog interrupt                                       */
#define  BSP_INT_POSITION_PVD                   1u   /* PVD through EXTI line 16 detection interrupt                    */
#define  BSP_INT_POSITION_TAMPER_STAMP          2u   /* Tamper & TimeStamp interrupts through the EXTI line 19          */
#define  BSP_INT_POSITION_RTC_WKUP              3u   /* RTC wakeup interrupt through the EXTI line 20                   */
#define  BSP_INT_POSITION_FLASH                 4u   /* FLASH global interrupt                                          */
#define  BSP_INT_POSITION_RCC                   5u   /* RCC global interrupt                                            */
#define  BSP_INT_POSITION_EXTI0                 6u   /* EXTI Line0 interrupt                                            */
#define  BSP_INT_POSITION_EXTI1                 7u   /* EXTI Line1 interrupt                                            */
#define  BSP_INT_POSITION_EXTI2                 8u   /* EXTI Line2                                                      */
#define  BSP_INT_POSITION_EXTI3                 9u   /* EXTI Line3 interrupt                                            */
#define  BSP_INT_POSITION_EXTI4                10u   /* EXTI Line4 interrupt                                            */
#define  BSP_INT_POSITION_DMA1_CH1             11u   /* DMA1 Channel 1 interrupt                                        */
#define  BSP_INT_POSITION_DMA1_CH2             12u   /* DMA1 Channel 2 interrupt                                        */
#define  BSP_INT_POSITION_DMA1_CH3             13u   /* DMA1 Channel 3 interrupt                                        */
#define  BSP_INT_POSITION_DMA1_CH4             14u   /* DMA1 Channel 4 interrupt                                        */
#define  BSP_INT_POSITION_DMA1_CH5             15u   /* DMA1 Channel 5 interrupt                                        */
#define  BSP_INT_POSITION_DMA1_CH6             16u   /* DMA1 Channel 6 interrupt                                        */
#define  BSP_INT_POSITION_DMA1_CH7             17u   /* DMA1 Channel 7 interrupt                                        */
#define  BSP_INT_POSITION_ADC1_2               18u   /* ADC1 & ADC2 global interrupt                                    */
#define  BSP_INT_POSITION_USB_HP               19u   /* USB High Priority interrupts                                    */
#define  BSP_INT_POSITION_USB_LP               20u   /* USB Low Priority interrupts                                     */
#define  BSP_INT_POSITION_FDCAN1_IT0           21u   /* FDCAN_IT0 interrupt                                             */
#define  BSP_INT_POSITION_FDCAN1_IT1           22u   /* FDCAN_IT1 interrupt                                             */
#define  BSP_INT_POSITION_EXTI9_5              23u   /* External Line[9:5] interrupts                                   */
#define  BSP_INT_POSITION_TIM1_BRK_TIM15       24u   /* TIM1 Break/TIM15 global interrupts                              */
#define  BSP_INT_POSITION_TIM1_UP_TIM16        25u   /* TIM1 Update/TIM10 global interrupts                             */
#define  BSP_INT_POSITION_TIM1_TRG_COM_TIM17   26u   /* TIM1 trigger & commutation/TIM17 interrupts                     */
#define  BSP_INT_POSITION_TIM1_CC              27u   /* TIM1 capture compare interrupt                                  */
#define  BSP_INT_POSITION_TIM2                 28u   /* TIM2 global interrupt                                           */
#define  BSP_INT_POSITION_TIM3                 29u   /* TIM3 global interrupt                                           */
#define  BSP_INT_POSITION_TIM4                 30u   /* TIM4 global interrupt                                           */
#define  BSP_INT_POSITION_I2C1_EV              31u   /* I2C1 event interrupt & EXTI Line23 interrupt                    */
#define  BSP_INT_POSITION_I2C1_ER              32u   /* I2C1 Error interrupt                                            */
#define  BSP_INT_POSITION_I2C2_EV              33u   /* I2C2 event interrupt & EXTI Line24 interrupt                    */
#define  BSP_INT_POSITION_I2C2_ER              34u   /* I2C2 Error interrupt                                            */
#define  BSP_INT_POSITION_SPI1                 35u   /* SPI1 global interrupt                                           */
#define  BSP_INT_POSITION_SPI2                 36u   /* SPI2 global interrupt                                           */
#define  BSP_INT_POSITION_USART1               37u   /* USART1 global interrupt & EXTI LINE 25                          */
#define  BSP_INT_POSITION_USART2               38u   /* USART2 global interrupt & EXTI LINE 26                          */
#define  BSP_INT_POSITION_USART3               39u   /* USART3 global interrupt & EXTI LINE 28                          */
#define  BSP_INT_POSITION_EXTI15_10            40u   /* EXTI Line[15:10] interrupts                                     */
#define  BSP_INT_POSITION_RTC_ALARM            41u   /* RTC alarms interrupt                                            */
#define  BSP_INT_POSITION_USB_WKUP             42u   /* USB Wakeup                                                      */
#define  BSP_INT_POSITION_TIM8_BRK             43u   /* TIM8 Break & Transition & Index error interrupt                 */
#define  BSP_INT_POSITION_TIM8_UP              44u   /* TIM8 update interrupt                                           */
#define  BSP_INT_POSITION_TIM8_TRG_COM         45u   /* TIM8 trigger & commutation, Direction Change, Index interrupt   */
#define  BSP_INT_POSITION_TIM8_CC              46u   /* TIM8 capture compare interrupt                                  */
#define  BSP_INT_POSITION_ADC3                 47u   /* ADC3 global interrupt                                           */
#define  BSP_INT_POSITION_FSMC                 48u   /* FSMC global interrupt                                           */
#define  BSP_INT_POSITION_LPTIM1               49u   /* LPTIM1 global interrupt                                         */
#define  BSP_INT_POSITION_TIM5                 50u   /* TIM1 global interrupt                                           */
#define  BSP_INT_POSITION_SPI3                 51u   /* SPI3 global interrupt                                           */
#define  BSP_INT_POSITION_UART4                52u   /* UART4 global interrupt                                          */
#define  BSP_INT_POSITION_UART5                53u   /* UART5 global interrupt                                          */
#define  BSP_INT_POSITION_TIM6_DAC1            54u   /* TIM6 global & DAC1 underrun interrupts                          */
#define  BSP_INT_POSITION_TIM7_DAC2            55u   /* TIM7 global & DAC2 underrun interrupts                          */
#define  BSP_INT_POSITION_DMA2_CH1             56u   /* DMA2 Channel 1 interrupt                                        */
#define  BSP_INT_POSITION_DMA2_CH2             57u   /* DMA2 Channel 2 interrupt                                        */
#define  BSP_INT_POSITION_DMA2_CH3             58u   /* DMA2 Channel 3 interrupt                                        */
#define  BSP_INT_POSITION_DMA2_CH4             59u   /* DMA2 Channel 4 interrupt                                        */
#define  BSP_INT_POSITION_DMA2_CH5             60u   /* DMA2 Channel 5 interrupt                                        */
#define  BSP_INT_POSITION_ADC4                 61u   /* ADC4 global interrupt                                           */
#define  BSP_INT_POSITION_ADC5                 62u   /* ADC5 global interrupt                                           */
#define  BSP_INT_POSITION_UCPD1                63u   /* UCPD1 global interrupt                                          */
#define  BSP_INT_POSITION_COMP1_3              64u   /* COMP1 to 3 combined with EXTI Lines 21, 22 & 29 interrupts      */
#define  BSP_INT_POSITION_COMP4_6              65u   /* COMP4 to COMP6 combined with EXTI 30, 31 & 32 interrupts        */
#define  BSP_INT_POSITION_COMP7                66u   /* COMP7 combined with EXTI 33 interrupt                           */
#define  BSP_INT_POSITION_HRTIM_MASTER_IRQn    67u   /* HRTIM master timer interrupt (hrtim_it1)                        */
#define  BSP_INT_POSITION_HRTIM_TIMA_IRQn      68u   /* HRTIM timer A interrupt (hrtim_it2)                             */
#define  BSP_INT_POSITION_HRTIM_TIMB_IRQn      69u   /* HRTIM timer B interrupt (hrtim_it3)                             */
#define  BSP_INT_POSITION_HRTIM_TIMC_IRQn      70u   /* HRTIM timer C interrupt (hrtim_it4)                             */
#define  BSP_INT_POSITION_HRTIM_TIMD_IRQn      71u   /* HRTIM timer D interrupt (hrtim_it5)                             */
#define  BSP_INT_POSITION_HRTIM_TIME_IRQn      72u   /* HRTIM timer E interrupt (hrtim_it6)                             */
#define  BSP_INT_POSITION_HRTIM_FLT_IRQn       73u   /* HRTIM fault interrupt (hrtim_it8)                               */
#define  BSP_INT_POSITION_HRTIM_TIMF_IRQn      74u   /* HRTIM timer F interrupt (hrtim_it7)                             */
#define  BSP_INT_POSITION_CRS                  75u   /* HRTIM timer F interrupt (hrtim_it7)                             */
#define  BSP_INT_POSITION_SAI                  76u   /* SAI interrupt                                                   */
#define  BSP_INT_POSITION_TIM20_BRK            77u   /* TIM20 Break & Transition & Index error interrupt                */
#define  BSP_INT_POSITION_TIM20_UP             78u   /* TIM20 update interrupt                                          */
#define  BSP_INT_POSITION_TIM20_TRG_COM        79u   /* TIM20 trigger & commutation, Direction Change, Index interrupt  */
#define  BSP_INT_POSITION_TIM20_CC             80u   /* TIM20 capture compare interrupt                                 */
#define  BSP_INT_POSITION_FPU                  81u   /* Floating point interrupt                                        */
#define  BSP_INT_POSITION_I2C4_EV              82u   /* I2C4 event interrupt & EXTI Line 42 interrupt                   */
#define  BSP_INT_POSITION_I2C4_ER              83u   /* I2C4 Error interrupt                                            */
#define  BSP_INT_POSITION_SPI4                 84u   /* SPI4 global interrupt                                           */
#define  BSP_INT_POSITION_AES                  85u   /* AES global interrupt                                            */
#define  BSP_INT_POSITION_FDCAN2_IT0           86u   /* FDCAN2_IT0 interrupt                                            */
#define  BSP_INT_POSITION_FDCAN2_IT1           87u   /* FDCAN2_IT1 interrupt                                            */
#define  BSP_INT_POSITION_FDCAN3_IT0           88u   /* FDCAN3_IT0 interrupt                                            */
#define  BSP_INT_POSITION_FDCAN3_IT1           89u   /* FDCAN3_IT1 interrupt                                            */
#define  BSP_INT_POSITION_RNG                  90u   /* RNG global interrupt                                            */
#define  BSP_INT_POSITION_LPUART1              91u   /* LPUART1 global interrupt                                        */
#define  BSP_INT_POSITION_I2C3_EV              92u   /* I2C3 event interrupt & EXTI Line 27 interrupt                   */
#define  BSP_INT_POSITION_I2C3_ER              93u   /* I2C3 Error interrupt                                            */
#define  BSP_INT_POSITION_DMAMUX_OVR           94u   /* DMAMUX Overrun interrupt                                        */
#define  BSP_INT_POSITION_QUADSPI              95u   /* QUADSPI global interrupt                                        */
#define  BSP_INT_POSITION_DMA1_CH8             96u   /* DMA1 Channel 8 interrupt                                        */
#define  BSP_INT_POSITION_DMA2_CH6             97u   /* DMA2 Channel 6 interrupt                                        */
#define  BSP_INT_POSITION_DMA2_CH7             98u   /* DMA2 Channel 7 interrupt                                        */
#define  BSP_INT_POSITION_DMA2_CH8             99u   /* DMA2 Channel 8 interrupt                                        */
#define  BSP_INT_POSITION_CORDIC              100u   /* Cordic interrupt                                                */
#define  BSP_INT_POSITION_FMAC                101u   /* FMAC interrupt                                                  */


void BSP_interrupts_init(void);

void BSP_interrupt_set_handler(uint16_t int_id, CPU_FNCT_VOID isr);

/*
 * IRQ handlers headers
 *
 */
void BSP_IntHandlerWWDG(void);

void BSP_IntHandlerPVD(void);

void BSP_IntHandlerTMP_STMP(void);

void BSP_IntHandlerRTC_WKUP(void);

void BSP_IntHandlerFLASH(void);

void BSP_IntHandlerRCC(void);

void BSP_IntHandlerEXTI0(void);

void BSP_IntHandlerEXTI1(void);

void BSP_IntHandlerEXTI2(void);

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

void BSP_IntHandlerUSB_HP(void);

void BSP_IntHandlerUSB_LP(void);

void BSP_IntHandlerFDCAN1_IT0(void);

void BSP_IntHandlerFDCAN1_IT1(void);

void BSP_IntHandlerEXTI9_5(void);

void BSP_IntHandlerTIM1_BRK_TIM15(void);

void BSP_IntHandlerTIM1_UP_TIM16(void);

void BSP_IntHandlerTIM1_TRG_COM_TIM17(void);

void BSP_IntHandlerTIM1_CC(void);

void BSP_IntHandlerTIM2(void);

void BSP_IntHandlerTIM3(void);

void BSP_IntHandlerTIM4(void);

void BSP_IntHandlerI2C1_EV(void);

void BSP_IntHandlerI2C1_ER(void);

void BSP_IntHandlerI2C2_EV(void);

void BSP_IntHandlerI2C2_ER(void);

void BSP_IntHandlerSPI1(void);

void BSP_IntHandlerSPI2(void);

void BSP_IntHandlerUSART1(void);

void BSP_IntHandlerUSART2(void);

void BSP_IntHandlerUSART3(void);

void BSP_IntHandlerEXTI15_10(void);

void BSP_IntHandlerRTC_ALARM(void);

void BSP_IntHandlerUSB_WKUP(void);

void BSP_IntHandlerTIM8_BRK(void);

void BSP_IntHandlerTIM8_UP(void);

void BSP_IntHandlerTIM8_TRG_COM(void);

void BSP_IntHandlerTIM8_CC(void);

void BSP_IntHandlerADC3(void);

void BSP_IntHandlerFSMC(void);

void BSP_IntHandlerLPTIM1(void);

void BSP_IntHandlerTIM5(void);

void BSP_IntHandlerSPI3(void);

void BSP_IntHandlerUART4(void);

void BSP_IntHandlerUART5(void);

void BSP_IntHandlerTIM6_DAC1(void);

void BSP_IntHandlerTIM7_DAC2(void);

void BSP_IntHandlerDMA2_CH1(void);

void BSP_IntHandlerDMA2_CH2(void);

void BSP_IntHandlerDMA2_CH3(void);

void BSP_IntHandlerDMA2_CH4(void);

void BSP_IntHandlerDMA2_CH5(void);

void BSP_IntHandlerADC4(void);

void BSP_IntHandlerADC5(void);

void BSP_IntHandlerUCPD1(void);

void BSP_IntHandlerCOMP1_3(void);

void BSP_IntHandlerCOMP4_6(void);

void BSP_IntHandlerCOMP7(void);

void BSP_IntHandlerHRTIM_MASTER_IRQn(void);

void BSP_IntHandlerHRTIM_TIMA_IRQn(void);

void BSP_IntHandlerHRTIM_TIMB_IRQn(void);

void BSP_IntHandlerHRTIM_TIMC_IRQn(void);

void BSP_IntHandlerHRTIM_TIMD_IRQn(void);

void BSP_IntHandlerHRTIM_TIME_IRQn(void);

void BSP_IntHandlerHRTIM_FLT_IRQn(void);

void BSP_IntHandlerHRTIM_TIMF_IRQn(void);

void BSP_IntHandlerCRS(void);

void BSP_IntHandlerSAI(void);

void BSP_IntHandlerTIM20_BRK(void);

void BSP_IntHandlerTIM20_UP(void);

void BSP_IntHandlerTIM20_TRG_COM(void);

void BSP_IntHandlerTIM20_CC(void);

void BSP_IntHandlerFPU(void);

void BSP_IntHandlerI2C4_EV(void);

void BSP_IntHandlerI2C4_ER(void);

void BSP_IntHandlerSPI4(void);

void BSP_IntHandlerAES(void);

void BSP_IntHandlerFDCAN2_IT0(void);

void BSP_IntHandlerFDCAN2_IT1(void);

void BSP_IntHandlerFDCAN3_IT0(void);

void BSP_IntHandlerFDCAN3_IT1(void);

void BSP_IntHandlerRNG(void);

void BSP_IntHandlerLPUART1(void);

void BSP_IntHandlerI2C3_EV(void);

void BSP_IntHandlerI2C3_ER(void);

void BSP_IntHandlerDMAMUX_OVR(void);

void BSP_IntHandlerQUADSPI(void);

void BSP_IntHandlerDMA1_CH8(void);

void BSP_IntHandlerDMA2_CH6(void);

void BSP_IntHandlerDMA2_CH7(void);

void BSP_IntHandlerDMA2_CH8(void);

void BSP_IntHandlerCORDIC(void);

void BSP_IntHandlerFMAC(void);

#endif //BSP_IRQ_MANAGER_H
