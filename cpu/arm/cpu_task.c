/*---------------------------------------------------------------------------*/
/* Akalon RTOS                                                               */
/* Copyright (c) 2011-2015, Dasa Dahanayaka                                  */
/* All rights reserved.                                                      */
/*                                                                           */
/* Usage of the works is permitted provided that this instrument is retained */
/* with the works, so that any entity that uses the works is notified of     */
/* this instrument.                                                          */
/*                                                                           */	
/* DISCLAIMER: THE WORKS ARE WITHOUT WARRANTY.                               */
/*                                                                           */
/*---------------------------------------------------------------------------*/

/*---------------------------------------------------------------------------*/
/*                                                                           */
/* File Name       : cpu_task.c                                              */
/* Description     : Akalon Task functions specific to the ARM processor     */
/* Notes           :                                                         */
/*                                                                           */
/*---------------------------------------------------------------------------*/
#include <akalon.h>
#include "../../kernel/kernel.h"
#include "../../kernel/cpu.h"

#define  CPU_TASK_RAM_SIZE   68   /* Regs r0-r15 + CPSR */


/*---------------------------------------------------------------------------*/
/*                                                                           */
/* Function Name   : cpu_task_ram_size                                       */
/* Description     : Return the size of a Task's CPU specific RAM.           */
/* Notes           : The CPU_TASK_RAM_SIZE is includes registers r0-15       */
/*                                                                           */
/*                                                                           */
/*                                                                           */
/*                                                                           */
/*---------------------------------------------------------------------------*/
usys     cpu_task_ram_size (void)
{
    return CPU_TASK_RAM_SIZE ;
} /* End of function cpu_task_ram_size() */



/*---------------------------------------------------------------------------*/
/*                                                                           */
/* Function Name   : cpu_task_ram_init                                       */
/* Description     : Initializes a Task's CPU specific RAM.                  */
/* Notes           :                                                         */
/*                                                                           */
/* When task_new() calls this function, this is how the Task's TCB looks...  */
/*                                                                           */
/*    ..   <-- End of the Task's TCB.                                        */
/*    ..                                                                     */
/*    ..                                                                     */
/*    ..   <-- stack_ptr                                                     */
/*    ..   <-- tcb_t end.                                                    */
/*    ..                                                                     */
/*    ..                                                                     */
/*    id   <-- tcb_t start (i.e. Start of the Task specific RAM)             */
/*                                                                           */
/* When this function returns, this is how the Task's TCB is going to look   */
/* like... BTW, the stack grows downwards.                                   */
/*                                                                           */
/*    ..   <-- End of the task's TCB (i.e 0xb16b00da, yes it's bigbooda)     */
/*    r14  <-- LR: Initialized with task_start()                             */
/*    r13  <-- SP: Points to the End of the task's TCB (i.e 2 notches above) */
/*    r12                                                                    */
/*    r11                                                                    */
/*    r10                                                                    */
/*    r9                                                                     */
/*    r8                                                                     */
/*    r7                                                                     */
/*    r6                                                                     */
/*    r5                                                                     */
/*    r4                                                                     */
/*    r3   <-- Arg 2 (4th Param)                                             */
/*    r2   <-- Arg 1 (3rd Param)                                             */
/*    r1   <-- Arg 0 (2nd Param)                                             */
/*    r0   <-- Task's Entry Point (1st Param). Also stack_ptr points here.   */
/*    cpsr <-- tcb->stack_ptr points here...                                 */
/*    ..                                                                     */
/*    ..                                                                     */
/*    ..   <-- tcb_t end.                                                    */
/*    ..                                                                     */
/*    ..                                                                     */
/*    id   <-- tcb_t start (i.e. Start of the Task specific RAM)             */
/*                                                                           */
/*---------------------------------------------------------------------------*/
void     cpu_task_ram_init (tcb_t *task, usys arg0, usys arg1, usys arg2)
{
    usys stack_size ; /* In Bytes */
    usys *tmp ;

    /* task->stack_ptr is pointing to memory just after the TCB. Set it */
    /* to the top of the stack space + CPU_TASK_RAM_SIZE. When the task */
    /* gets activated, it's stack will be pointing to this location.    */

    stack_size = task->stack_size + CPU_TASK_RAM_SIZE ;
    task->stack_ptr += (stack_size / sizeof (usys)) ; 
    task->stack_ptr -- ;

    tmp = task->stack_ptr ; /* Top of stack */

    *task->stack_ptr-- = 0xb16b00da ; /* First Item */

    /* Set r15/r14 (i.e. PC and LR) to task_start() */
    *task->stack_ptr-- = (usys) task_start ;
    *task->stack_ptr-- = (usys) task_start ;

    /* Set r13 (i.e. stack pointer) to the top of the stack */
    *task->stack_ptr   = (usys) tmp ;

    /* From r12 - r4 is all zeros */

    task->stack_ptr   -= 10   ;   /* Point to r3 */
    *task->stack_ptr-- = arg2 ;   /* r3 */ 
    *task->stack_ptr-- = arg1 ;   /* r2 */
    *task->stack_ptr-- = arg0 ;   /* r1 */

    /* Set r0 which is the Task Entry Point */
    *task->stack_ptr-- = (usys) task->func ; 

    /*----------------------------------------*/
    /* Set the CPSR...                        */
    /*                                        */
    /* Bit 04-00 : M4 - M0                    */
    /*             10011 - SVC (Supervisor)   */
    /* Bit 05    : T (1 - Thumb Enabled)      */
    /* Bit 06    : F (1 - FIRQ Disabled)      */
    /* Bit 07    : I (1 - IRQ Disabled)       */
    /* Bit 28    : V (1 - Overflow)           */
    /* Bit 29    : C (1 - Carry)              */
    /* Bit 30    : Z (1 - Zero)               */
    /* Bit 31    : N (1 - Negative)           */
    /*----------------------------------------*/

    *task->stack_ptr = 0xd3 ; 

} /* End of function cpu_task_ram_init() */

