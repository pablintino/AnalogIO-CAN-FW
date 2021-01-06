/**
  ******************************************************************************
  * @file      startup_stm32g431xx.s
  * @author    MCD Application Team
  * @brief     STM32G431xx devices vector table GCC toolchain.
  *            This module performs:
  *                - Set the initial SP
  *                - Set the initial PC == Reset_Handler,
  *                - Set the vector table entries with the exceptions ISR address,
  *                - Configure the clock system
  *                - Branches to main in the C library (which eventually
  *                  calls main()).
  *            After Reset the Cortex-M4 processor is in Thread mode,
  *            priority is Privileged, and the Stack is set to Main.
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; Copyright (c) 2019 STMicroelectronics.
  * All rights reserved.</center></h2>
  *
  * This software component is licensed by ST under BSD 3-Clause license,
  * the "License"; You may not use this file except in compliance with the
  * License. You may obtain a copy of the License at:
  *                        opensource.org/licenses/BSD-3-Clause
  *
  ******************************************************************************
  */

  .syntax unified
	.cpu cortex-m4
	.fpu softvfp
	.thumb

.global	g_pfnVectors
.global	Default_Handler

/* start address for the initialization values of the .data section.
defined in linker script */
.word	_sidata
/* start address for the .data section. defined in linker script */
.word	_sdata
/* end address for the .data section. defined in linker script */
.word	_edata
/* start address for the .bss section. defined in linker script */
.word	_sbss
/* end address for the .bss section. defined in linker script */
.word	_ebss

.equ  BootRAM,        0xF1E0F85F
/**
 * @brief  This is the code that gets called when the processor first
 *          starts execution following a reset event. Only the absolutely
 *          necessary set is performed, after which the application
 *          supplied main() routine is called.
 * @param  None
 * @retval : None
*/

    .section	.text.Reset_Handler
	.weak	Reset_Handler
	.type	Reset_Handler, %function
Reset_Handler:
  ldr   r0, =_estack
  mov   sp, r0          /* set stack pointer */

/* Copy the data segment initializers from flash to SRAM */
  ldr r0, =_sdata
  ldr r1, =_edata
  ldr r2, =_sidata
  movs r3, #0
  b	LoopCopyDataInit

CopyDataInit:
  ldr r4, [r2, r3]
  str r4, [r0, r3]
  adds r3, r3, #4

LoopCopyDataInit:
  adds r4, r0, r3
  cmp r4, r1
  bcc CopyDataInit
  
/* Zero fill the bss segment. */
  ldr r2, =_sbss
  ldr r4, =_ebss
  movs r3, #0
  b LoopFillZerobss

FillZerobss:
  str  r3, [r2]
  adds r2, r2, #4

LoopFillZerobss:
  cmp r2, r4
  bcc FillZerobss

/* Call the clock system initialization function.*/
    bl  SystemInit
/* Call static constructors */
    bl __libc_init_array
/* Call the application's entry point.*/
	bl	main

LoopForever:
    b LoopForever

.size	Reset_Handler, .-Reset_Handler

/**
 * @brief  This is the code that gets called when the processor receives an
 *         unexpected interrupt.  This simply enters an infinite loop, preserving
 *         the system state for examination by a debugger.
 *
 * @param  None
 * @retval : None
*/
    .section	.text.Default_Handler,"ax",%progbits
Default_Handler:
Infinite_Loop:
	b	Infinite_Loop
	.size	Default_Handler, .-Default_Handler
/******************************************************************************
*
* The minimal vector table for a Cortex-M4.  Note that the proper constructs
* must be placed on this to ensure that it ends up at physical address
* 0x0000.0000.
*
******************************************************************************/
 	.section	.isr_vector,"a",%progbits
	.type	g_pfnVectors, %object
	.size	g_pfnVectors, .-g_pfnVectors


g_pfnVectors:
	.word	_estack
	.word	Reset_Handler
	.word	NMI_Handler
	.word	HardFault_Handler
	.word	MemManage_Handler
	.word	BusFault_Handler
	.word	UsageFault_Handler
	.word	0
	.word	0
	.word	0
	.word	0
	.word	SVC_Handler
	.word	DebugMon_Handler
	.word	0
	.word	OS_CPU_PendSVHandler
	.word	OS_CPU_SysTickHandler
	.word	BSP_IntHandlerWWDG
	.word	BSP_IntHandlerPVD
	.word	BSP_IntHandlerTMP_STMP
	.word	BSP_IntHandlerRTC_WKUP
	.word	BSP_IntHandlerFLASH
	.word	BSP_IntHandlerRCC
	.word	BSP_IntHandlerEXTI0
	.word	BSP_IntHandlerEXTI1
	.word	BSP_IntHandlerEXTI2
	.word	BSP_IntHandlerEXTI3
	.word	BSP_IntHandlerEXTI4
	.word	BSP_IntHandlerDMA1_CH1
	.word	BSP_IntHandlerDMA1_CH2
	.word	BSP_IntHandlerDMA1_CH3
	.word	BSP_IntHandlerDMA1_CH4
	.word	BSP_IntHandlerDMA1_CH5
	.word	BSP_IntHandlerDMA1_CH6
	.word	0
	.word	BSP_IntHandlerADC1_2
	.word	BSP_IntHandlerUSB_HP
	.word	BSP_IntHandlerUSB_LP
	.word	BSP_IntHandlerFDCAN1_IT0
	.word	BSP_IntHandlerFDCAN1_IT1
	.word	BSP_IntHandlerEXTI9_5
	.word	BSP_IntHandlerTIM1_BRK_TIM15
	.word	BSP_IntHandlerTIM1_UP_TIM16
	.word	BSP_IntHandlerTIM1_TRG_COM_TIM17
	.word	BSP_IntHandlerTIM1_CC
	.word	BSP_IntHandlerTIM2
	.word	BSP_IntHandlerTIM3
	.word	BSP_IntHandlerTIM4
	.word	BSP_IntHandlerI2C1_EV
	.word	BSP_IntHandlerI2C2_ER
	.word	BSP_IntHandlerI2C2_EV
	.word	BSP_IntHandlerI2C2_ER
	.word	BSP_IntHandlerSPI1
	.word	BSP_IntHandlerSPI2
	.word	BSP_IntHandlerUSART1
	.word	BSP_IntHandlerUSART2
	.word	BSP_IntHandlerUSART3
	.word	BSP_IntHandlerEXTI15_10
	.word	BSP_IntHandlerRTC_ALARM
	.word	BSP_IntHandlerUSB_WKUP
	.word	BSP_IntHandlerTIM8_BRK
	.word	BSP_IntHandlerTIM8_UP
	.word	BSP_IntHandlerTIM8_TRG_COM
	.word	BSP_IntHandlerTIM8_CC
	.word	0
	.word	0
	.word	BSP_IntHandlerLPTIM1
	.word	0
	.word	BSP_IntHandlerSPI3
	.word	BSP_IntHandlerUART4
	.word	0
	.word	BSP_IntHandlerTIM6_DAC1
	.word	BSP_IntHandlerTIM7_DAC2
	.word	BSP_IntHandlerDMA2_CH1
	.word	BSP_IntHandlerDMA2_CH2
	.word	BSP_IntHandlerDMA2_CH3
	.word	BSP_IntHandlerDMA2_CH4
	.word	BSP_IntHandlerDMA2_CH5
	.word	0
	.word	0
	.word	BSP_IntHandlerUCPD1
	.word	BSP_IntHandlerCOMP1_3
	.word	BSP_IntHandlerCOMP4
	.word	0
	.word	0
	.word	0
	.word	0
	.word	0
	.word	0
	.word	0
	.word	0
	.word	0
	.word	BSP_IntHandlerCRS
	.word	BSP_IntHandlerSAI
	.word	0
	.word	0
	.word	0
	.word	0
	.word	BSP_IntHandlerFPU
	.word	0
	.word	0
	.word	0
	.word	0
	.word	0
	.word	0
	.word	0
	.word	0
	.word	BSP_IntHandlerRNG
	.word	BSP_IntHandlerLPUART1
	.word	BSP_IntHandlerI2C3_EV
	.word	BSP_IntHandlerI2C3_ER
	.word	BSP_IntHandlerDMAMUX_OVR
	.word	0
	.word	0
	.word	BSP_IntHandlerDMA2_CH6
	.word	0
	.word	0
	.word	BSP_IntHandlerCORDIC
	.word	BSP_IntHandlerFMAC

/*******************************************************************************
*
* Provide weak aliases for each Exception handler to the Default_Handler.
* As they are weak aliases, any function with the same name will override
* this definition.
*
*******************************************************************************/

	.weak	NMI_Handler
	.thumb_set NMI_Handler,Default_Handler

	.weak	HardFault_Handler
	.thumb_set HardFault_Handler,Default_Handler

	.weak	MemManage_Handler
	.thumb_set MemManage_Handler,Default_Handler

	.weak	BusFault_Handler
	.thumb_set BusFault_Handler,Default_Handler

	.weak	UsageFault_Handler
	.thumb_set UsageFault_Handler,Default_Handler

	.weak	SVC_Handler
	.thumb_set SVC_Handler,Default_Handler

	.weak	DebugMon_Handler
	.thumb_set DebugMon_Handler,Default_Handler

	.weak	OS_CPU_PendSVHandler
	.thumb_set OS_CPU_PendSVHandler,Default_Handler

	.weak	OS_CPU_SysTickHandler
	.thumb_set OS_CPU_SysTickHandler,Default_Handler

	.weak	BSP_IntHandlerWWDG
	.thumb_set BSP_IntHandlerWWDG,Default_Handler

	.weak	BSP_IntHandlerPVD
	.thumb_set BSP_IntHandlerPVD,Default_Handler

	.weak	BSP_IntHandlerTAMPER_STAMP
	.thumb_set BSP_IntHandlerTMP_STMP,Default_Handler

	.weak	BSP_IntHandlerRTC_WKUP
	.thumb_set BSP_IntHandlerRTC_WKUP,Default_Handler

	.weak	BSP_IntHandlerFLASH
	.thumb_set BSP_IntHandlerFLASH,Default_Handler

	.weak	BSP_IntHandlerRCC
	.thumb_set BSP_IntHandlerRCC,Default_Handler

	.weak	BSP_IntHandlerEXTI0
	.thumb_set BSP_IntHandlerEXTI0,Default_Handler

	.weak	BSP_IntHandlerEXTI1
	.thumb_set BSP_IntHandlerEXTI1,Default_Handler

	.weak	BSP_IntHandlerEXTI2
	.thumb_set BSP_IntHandlerEXTI2,Default_Handler

	.weak	BSP_IntHandlerEXTI3
	.thumb_set BSP_IntHandlerEXTI3,Default_Handler

	.weak	BSP_IntHandlerEXTI4
	.thumb_set BSP_IntHandlerEXTI4,Default_Handler

	.weak	BSP_IntHandlerDMA1_CH1
	.thumb_set BSP_IntHandlerDMA1_CH1,Default_Handler

	.weak	BSP_IntHandlerDMA1_CH2
	.thumb_set BSP_IntHandlerDMA1_CH2,Default_Handler

	.weak	BSP_IntHandlerDMA1_CH3
	.thumb_set BSP_IntHandlerDMA1_CH3,Default_Handler

	.weak	BSP_IntHandlerDMA1_CH4
	.thumb_set BSP_IntHandlerDMA1_CH4,Default_Handler

	.weak	BSP_IntHandlerDMA1_CH5
	.thumb_set BSP_IntHandlerDMA1_CH5,Default_Handler

	.weak	BSP_IntHandlerDMA1_CH6
	.thumb_set BSP_IntHandlerDMA1_CH6,Default_Handler

	.weak	BSP_IntHandlerADC1_2
	.thumb_set BSP_IntHandlerADC1_2,Default_Handler

	.weak	BSP_IntHandlerUSB_HP
	.thumb_set BSP_IntHandlerUSB_HP,Default_Handler

	.weak	BSP_IntHandlerUSB_LP
	.thumb_set BSP_IntHandlerUSB_LP,Default_Handler

	.weak	BSP_IntHandlerFDCAN1_IT0
	.thumb_set BSP_IntHandlerFDCAN1_IT0,Default_Handler

	.weak	BSP_IntHandlerFDCAN1_IT1
	.thumb_set BSP_IntHandlerFDCAN1_IT1,Default_Handler

	.weak	BSP_IntHandlerEXTI9_5
	.thumb_set BSP_IntHandlerEXTI9_5,Default_Handler

	.weak	BSP_IntHandlerTIM1_BRK_TIM15
	.thumb_set BSP_IntHandlerTIM1_BRK_TIM15,Default_Handler

	.weak	BSP_IntHandlerTIM1_UP_TIM16
	.thumb_set BSP_IntHandlerTIM1_UP_TIM16,Default_Handler

	.weak	BSP_IntHandlerTIM1_TRG_COM_TIM17
	.thumb_set BSP_IntHandlerTIM1_TRG_COM_TIM17,Default_Handler

	.weak	BSP_IntHandlerTIM1_CC
	.thumb_set BSP_IntHandlerTIM1_CC,Default_Handler

	.weak	BSP_IntHandlerTIM2
	.thumb_set BSP_IntHandlerTIM2,Default_Handler

	.weak	BSP_IntHandlerTIM3
	.thumb_set BSP_IntHandlerTIM3,Default_Handler

	.weak	BSP_IntHandlerTIM4
	.thumb_set BSP_IntHandlerTIM4,Default_Handler

	.weak	BSP_IntHandlerI2C1_EV
	.thumb_set BSP_IntHandlerI2C1_EV,Default_Handler

	.weak	BSP_IntHandlerI2C1_ER
	.thumb_set BSP_IntHandlerI2C1_ER,Default_Handler

	.weak	BSP_IntHandlerI2C2_EV
	.thumb_set BSP_IntHandlerI2C2_EV,Default_Handler

	.weak	BSP_IntHandlerI2C2_ER
	.thumb_set BSP_IntHandlerI2C2_ER,Default_Handler

	.weak	BSP_IntHandlerSPI1
	.thumb_set BSP_IntHandlerSPI1,Default_Handler

	.weak	BSP_IntHandlerSPI2
	.thumb_set BSP_IntHandlerSPI2,Default_Handler

	.weak	BSP_IntHandlerUSART1
	.thumb_set BSP_IntHandlerUSART1,Default_Handler

	.weak	BSP_IntHandlerUSART2
	.thumb_set BSP_IntHandlerUSART2,Default_Handler

	.weak	BSP_IntHandlerUSART3
	.thumb_set BSP_IntHandlerUSART3,Default_Handler

	.weak	BSP_IntHandlerEXTI15_10
	.thumb_set BSP_IntHandlerEXTI15_10,Default_Handler

	.weak	BSP_IntHandlerRTC_ALARM
	.thumb_set BSP_IntHandlerRTC_ALARM,Default_Handler

	.weak	BSP_IntHandlerUSB_WKUP
	.thumb_set BSP_IntHandlerUSB_WKUP,Default_Handler

	.weak	BSP_IntHandlerTIM8_BRK
	.thumb_set BSP_IntHandlerTIM8_BRK,Default_Handler

	.weak	BSP_IntHandlerTIM8_UP
	.thumb_set BSP_IntHandlerTIM8_UP,Default_Handler

	.weak	BSP_IntHandlerTIM8_TRG_COM
	.thumb_set BSP_IntHandlerTIM8_TRG_COM,Default_Handler

	.weak	BSP_IntHandlerTIM8_CC
	.thumb_set BSP_IntHandlerTIM8_CC,Default_Handler

	.weak	BSP_IntHandlerLPTIM1
	.thumb_set BSP_IntHandlerLPTIM1,Default_Handler

	.weak	BSP_IntHandlerSPI3
	.thumb_set BSP_IntHandlerSPI3,Default_Handler

	.weak	BSP_IntHandlerUART4
	.thumb_set BSP_IntHandlerUART4,Default_Handler

	.weak	BSP_IntHandlerTIM6_DAC1
	.thumb_set BSP_IntHandlerTIM6_DAC1,Default_Handler

	.weak	BSP_IntHandlerTIM7_DAC2
	.thumb_set BSP_IntHandlerTIM7_DAC2,Default_Handler

	.weak	BSP_IntHandlerDMA2_CH1
	.thumb_set BSP_IntHandlerDMA2_CH1,Default_Handler

	.weak	BSP_IntHandlerDMA2_CH2
	.thumb_set BSP_IntHandlerDMA2_CH2,Default_Handler

	.weak	BSP_IntHandlerDMA2_CH3
	.thumb_set BSP_IntHandlerDMA2_CH3,Default_Handler

	.weak	BSP_IntHandlerDMA2_CH4
	.thumb_set BSP_IntHandlerDMA2_CH4,Default_Handler

	.weak	BSP_IntHandlerDMA2_CH5
	.thumb_set BSP_IntHandlerDMA2_CH5,Default_Handler

	.weak	BSP_IntHandlerUCPD1
	.thumb_set BSP_IntHandlerUCPD1,Default_Handler

	.weak	BSP_IntHandlerCOMP1_3
	.thumb_set BSP_IntHandlerCOMP1_3,Default_Handler

	.weak	BSP_IntHandlerCOMP4
	.thumb_set BSP_IntHandlerCOMP4,Default_Handler

	.weak	BSP_IntHandlerCRS
	.thumb_set BSP_IntHandlerCRS,Default_Handler

	.weak	BSP_IntHandlerSAI
	.thumb_set BSP_IntHandlerSAI,Default_Handler

	.weak	BSP_IntHandlerFPU
	.thumb_set BSP_IntHandlerFPU,Default_Handler

	.weak	BSP_IntHandlerRNG
	.thumb_set BSP_IntHandlerRNG,Default_Handler

	.weak	BSP_IntHandlerLPUART1
	.thumb_set BSP_IntHandlerLPUART1,Default_Handler

	.weak	BSP_IntHandlerI2C3_EV
	.thumb_set BSP_IntHandlerI2C3_EV,Default_Handler

	.weak	I2C3_ER_IRQHandler
	.thumb_set I2C3_ER_IRQHandler,Default_Handler

	.weak	BSP_IntHandlerI2C3_ER
	.thumb_set BSP_IntHandlerI2C3_ER,Default_Handler

	.weak	BSP_IntHandlerDMA2_CH6
	.thumb_set DMA2_Channel6_IRQHandler,Default_Handler

	.weak	BSP_IntHandlerCORDIC
	.thumb_set BSP_IntHandlerCORDIC,Default_Handler

	.weak	BSP_IntHandlerFMAC
	.thumb_set BSP_IntHandlerFMAC,Default_Handler

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
