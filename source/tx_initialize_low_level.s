// by default AzureRTOS is configured to use static byte pool for
// allocation, in case dynamic allocation is to be used, uncomment
// the define below and update the linker files to define the following symbols
// EWARM toolchain:
//       place in RAM_region    { last section FREE_MEM};
// MDK-ARM toolchain;
//       either define the RW_IRAM1 region in the ".sct" file or modify this file by referring to the correct memory region.
//         LDR r1, =|Image$$RW_IRAM1$$ZI$$Limit|
// STM32CubeIDE toolchain:
//       ._threadx_heap :
//       {
//        . = ALIGN(8);
//        __RAM_segment_used_end__ = .;
//        . = . + 64K;
//        . = ALIGN(8);
//       } >RAM_D1 AT> RAM_D1
//  The simplest way to provide memory for ThreadX is to define a new section, see ._threadx_heap above.
//  In the example above the ThreadX heap size is set to 64KBytes.
//  The ._threadx_heap must be located between the .bss and the ._user_heap_stack sections in the linker script.
//  Caution: Make sure that ThreadX does not need more than the provided heap memory (64KBytes in this example).
//  Read more in STM32CubeIDE User Guide, chapter: "Linker script".

@/**************************************************************************/
@/*                                                                        */
@/*       Copyright (c) Microsoft Corporation. All rights reserved.        */
@/*                                                                        */
@/*       This software is licensed under the Microsoft Software License   */
@/*       Terms for Microsoft Azure RTOS. Full text of the license can be  */
@/*       found in the LICENSE file at https://aka.ms/AzureRTOS_EULA       */
@/*       and in the root directory of this software.                      */
@/*                                                                        */
@/**************************************************************************/
@
@
@/**************************************************************************/
@/**************************************************************************/
@/**                                                                       */
@/** ThreadX Component                                                     */
@/**                                                                       */
@/**   Initialize                                                          */
@/**                                                                       */
@/**************************************************************************/
@/**************************************************************************/
@
@
    .global     _tx_thread_system_stack_ptr
    .global     _tx_initialize_unused_memory
    .global     __RAM_segment_used_end__
    .global     _tx_timer_interrupt
    .global     __main
    .global     __tx_SVCallHandler
    .global     __tx_PendSVHandler
    .global     _vectors
    .global     __tx_NMIHandler                     @ NMI
    .global     __tx_BadHandler                     @ HardFault
    .global     __tx_SVCallHandler                  @ SVCall
    .global     __tx_DBGHandler                     @ Monitor
    .global     __tx_PendSVHandler                  @ PendSV
    .global     __tx_SysTickHandler                 @ SysTick
    .global     __tx_IntHandler                     @ Int 0
@
@

    .text 32
    .align 4
    .syntax unified
@/**************************************************************************/
@/*                                                                        */
@/*  FUNCTION                                               RELEASE        */
@/*                                                                        */
@/*    _tx_initialize_low_level                          Cortex-M4/GNU     */
@/*                                                           6.1          */
@/*  AUTHOR                                                                */
@/*                                                                        */
@/*    William E. Lamie, Microsoft Corporation                             */
@/*                                                                        */
@/*  DESCRIPTION                                                           */
@/*                                                                        */
@/*    This function is responsible for any low-level processor            */
@/*    initialization, including setting up interrupt vectors, setting     */
@/*    up a periodic timer interrupt source, saving the system stack       */
@/*    pointer for use in ISR processing later, and finding the first      */
@/*    available RAM memory address for tx_application_define.             */
@/*                                                                        */
@/*  INPUT                                                                 */
@/*                                                                        */
@/*    None                                                                */
@/*                                                                        */
@/*  OUTPUT                                                                */
@/*                                                                        */
@/*    None                                                                */
@/*                                                                        */
@/*  CALLS                                                                 */
@/*                                                                        */
@/*    None                                                                */
@/*                                                                        */
@/*  CALLED BY                                                             */
@/*                                                                        */
@/*    _tx_initialize_kernel_enter           ThreadX entry function        */
@/*                                                                        */
@/*  RELEASE HISTORY                                                       */
@/*                                                                        */
@/*    DATE              NAME                      DESCRIPTION             */
@/*                                                                        */
@/*  05-19-2020     William E. Lamie         Initial Version 6.0           */
@/*  09-30-2020     William E. Lamie         Modified Comment(s), fixed    */
@/*                                            GNU assembly comment, clean */
@/*                                            up whitespace, resulting    */
@/*                                            in version 6.1              */
@/*                                                                        */
@/**************************************************************************/
@VOID   _tx_initialize_low_level(VOID)
@{
    .global  _tx_initialize_low_level
    .thumb_func
_tx_initialize_low_level:
@
@    /* Disable interrupts during ThreadX initialization.  */
@
    CPSID   i
@
@    /* Set base of available memory to end of non-initialised RAM area.  */
@
#ifdef USE_DYNAMIC_MEMORY_ALLOCATION
    LDR     r0, =_tx_initialize_unused_memory       @ Build address of unused memory pointer
    LDR     r1, =__RAM_segment_used_end__           @ Build first free address
    ADD     r1, r1, #4                              @
    STR     r1, [r0]                                @ Setup first unused memory pointer
#endif
@
@    /* Setup Vector Table Offset Register.  */
@
    MOV     r0, #0xE000E000                         @ Build address of NVIC registers
    LDR     r1, =g_pfnVectors                           @ Pickup address of vector table
    STR     r1, [r0, #0xD08]                        @ Set vector table address
@
@    /* Set system stack pointer from vector value.  */
@
    LDR     r0, =_tx_thread_system_stack_ptr        @ Build address of system stack pointer
    LDR     r1, =g_pfnVectors                           @ Pickup address of vector table
    LDR     r1, [r1]                                @ Pickup reset stack pointer
    STR     r1, [r0]                                @ Save system stack pointer
@
@    /* Enable the cycle count register.  */
@
    LDR     r0, =0xE0001000                         @ Build address of DWT register
    LDR     r1, [r0]                                @ Pickup the current value
    ORR     r1, r1, #1                              @ Set the CYCCNTENA bit
    STR     r1, [r0]                                @ Enable the cycle count register

@
@    /* Configure handler priorities.  */
@
    MOV     r0, #0xE000E000                         @ Build address of NVIC registers
    LDR     r1, =0x00000000                         @ Rsrv, UsgF, BusF, MemM
    STR     r1, [r0, #0xD18]                        @ Setup System Handlers 4-7 Priority Registers

    LDR     r1, =0xFF000000                         @ SVCl, Rsrv, Rsrv, Rsrv
    STR     r1, [r0, #0xD1C]                        @ Setup System Handlers 8-11 Priority Registers
                                                    @ Note: SVC must be lowest priority, which is 0xFF

    LDR     r1, =0x40FF0000                         @ SysT, PnSV, Rsrv, DbgM
    STR     r1, [r0, #0xD20]                        @ Setup System Handlers 12-15 Priority Registers
                                                    @ Note: PnSV must be lowest priority, which is 0xFF
@
@    /* Return to caller.  */
@
    BX      lr
@}
@

@/* Define shells for each of the unused vectors.  */
@
    .global  __tx_BadHandler
    .thumb_func
__tx_BadHandler:
    B       __tx_BadHandler

@ /* added to catch the hardfault */

    .global  __tx_HardfaultHandler
    .thumb_func
__tx_HardfaultHandler:
    B       __tx_HardfaultHandler

@ /* added to catch the SVC */

    .global  __tx_SVCallHandler
    .thumb_func
__tx_SVCallHandler:
    B       __tx_SVCallHandler

@ /* Generic interrupt handler template */
    .global  __tx_IntHandler
    .thumb_func
__tx_IntHandler:
@ VOID InterruptHandler (VOID)
@ {
    PUSH    {r0, lr}
#ifdef TX_EXECUTION_PROFILE_ENABLE
    BL      _tx_execution_isr_enter             @ Call the ISR enter function
#endif

@    /* Do interrupt handler work here */
@    /* BL <your C Function>.... */

#ifdef TX_EXECUTION_PROFILE_ENABLE
    BL      _tx_execution_isr_exit              @ Call the ISR exit function
#endif
    POP     {r0, lr}
    BX      LR
@ }

@ /* System Tick timer interrupt handler */
    .global  __tx_SysTickHandler
    .global  SysTick_Handler
    .thumb_func
__tx_SysTickHandler:
    .thumb_func
SysTick_Handler:
@ VOID TimerInterruptHandler (VOID)
@ {
@
    PUSH    {r0, lr}
#ifdef TX_EXECUTION_PROFILE_ENABLE
    BL      _tx_execution_isr_enter             @ Call the ISR enter function
#endif
    BL      _tx_timer_interrupt
#ifdef TX_EXECUTION_PROFILE_ENABLE
    BL      _tx_execution_isr_exit              @ Call the ISR exit function
#endif
    POP     {r0, lr}
    BX      LR
@ }

@ /* NMI, DBG handlers */
    .global  __tx_NMIHandler
    .thumb_func
__tx_NMIHandler:
    B       __tx_NMIHandler

    .global  __tx_DBGHandler
    .thumb_func
__tx_DBGHandler:
    B       __tx_DBGHandler
