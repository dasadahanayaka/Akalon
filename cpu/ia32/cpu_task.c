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
/* Description     : Akalon Task functions specific to the IA32 processor    */
/* Notes           :                                                         */
/*                                                                           */
/*---------------------------------------------------------------------------*/
#include <akalon.h>
#include "../../kernel/kernel.h"
#include "../../kernel/cpu.h"


/*---------------------------------------------------------------------------*/
/*                                                                           */
/* This is how CPU_TASK_RAM_SIZE is calculated....                           */
/*                                                                           */
/*    01 0xb1gb00da (End of the Task's Task).                                */
/*    02 Arg 2 (4th Param).                                                  */
/*    03 Arg 1 (3rd Param)                                                   */
/*    04 Arg 0 (2nd Param)                                                   */
/*    05 Caller's Entry Point (1st Param)                                    */
/*    06 Dummy Return Address                                                */
/*    07 Address of task_start()                                             */
/*    08 eflags                                                              */
/*    09 eax                                                                 */
/*    10 ecx                                                                 */
/*    11 edx                                                                 */
/*    12 ebx                                                                 */
/*    13 <esp>                                                               */
/*    14 ebp                                                                 */
/*    15 esi                                                                 */
/*    16 edi                                                                 */
/*                                                                           */
/*    + During an interrupt, the processor saves the following...            */
/*                                                                           */
/*    17 eflag                                                               */
/*    18 cs                                                                  */
/*    19 eip                                                                 */
/*                                                                           */
/*    + ISR amount (which is a guess)                                        */
/*                                                                           */
/*---------------------------------------------------------------------------*/
#define  STACK_FOR_INTERRUPT 84 /* In Bytes */
#define  CPU_TASK_RAM_SIZE   ((19 * 4) + STACK_FOR_INTERRUPT) 


/*---------------------------------------------------------------------------*/
/*                                                                           */
/* Function Name   : cpu_task_ram_size                                       */
/* Description     : Return the size of a Task's CPU specific RAM.           */
/* Notes           :                                                         */
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
/*                                                                           */
/* Notes           :                                                         */
/*                                                                           */
/* When task_new() calls this function, this is how the Task's TCB looks...  */
/*                                                                           */
/*    ..     <-- End of the Task's TCB.                                      */
/*    ..                                                                     */
/*    ..                                                                     */
/*    ..     <-- stack_ptr                                                   */
/*    ..     <-- tcb_t end.                                                  */
/*    ..                                                                     */
/*    ..                                                                     */
/*    id     <-- tcb_t start (i.e. Start of the Task specific RAM)           */
/*                                                                           */
/* When this function returns, this is how the Task's TCB and it's stack is  */
/* going to look like...BTW, the stack grows downwards.                      */
/*                                                                           */
/*    ..      <-- 0xb1gb00da (End of the Task's Task).                       */
/*    ..      <-- Arg 2 (4th Param).                                         */
/*    ..      <-- Arg 1 (3rd Param)                                          */
/*    ..      <-- Arg 0 (2nd Param)                                          */
/*    ..      <-- Caller's Entry Point (1st Param)                           */
/*    ..      <-- Dummy Return Address                                       */
/*    eip     <-- Address of task_start()                                    */
/*    eflags                                                                 */
/*    eax                                                                    */
/*    ecx                                                                    */
/*    edx                                                                    */
/*    ebx                                                                    */
/*    <esp>   <-- Points to 4 bytes above the eax                            */
/*    ebp                                                                    */
/*    esi                                                                    */
/*    edi     <-- task->stack_ptr                                            */
/*    ..                                                                     */
/*    ..                                                                     */
/*    ..                                                                     */
/*    ..      <-- tcb_t end                                                  */
/*    ..                                                                     */
/*    ..                                                                     */
/*    id      <-- tcb_t start                                                */
/*                                                                           */
/* The task is activated by cpu_task_switch() by...                          */
/*                                                                           */
/* a. Loading the task->stack_ptr into esp.                                  */
/* b. A "popal" instruction which will load all the general registers        */
/* c. A "popfl" instruction which will load the eflags register.             */
/* b. A "ret" instruction which will start executing task_start(). When in   */
/*    task_start(), the stack (i.e esp) will point to the Dummy Return       */
/*    Address.                                                               */
/*                                                                           */
/*---------------------------------------------------------------------------*/
void     cpu_task_ram_init (tcb_t *task, usys arg0, usys arg1, usys arg2)
{
    usys stack_size ; /* In Bytes */

    /* Point to the top of the Task's Stack */
    stack_size = task->stack_size + CPU_TASK_RAM_SIZE ; 
    task->stack_ptr += (stack_size / sizeof (usys)) ;
    task->stack_ptr -- ; 

    *task->stack_ptr-- = 0xb16b00da ; /* First Item */

    /* Copy the inputs for task_start() onto the stack */
    *task->stack_ptr-- = arg2 ;
    *task->stack_ptr-- = arg1 ;
    *task->stack_ptr-- = arg0 ;

    /* Now put the Caller's entry point on the stack */
    *task->stack_ptr-- = (usys) task->func ;

    /* ..and the dummy return address */ 
    task->stack_ptr -- ;

    /* ..and the addr of task_start() */
    *task->stack_ptr-- = (usys) task_start ;

    /* Update the stack pointer to point 20 bytes below...*/

    task->stack_ptr -= 5 ; /* <esp> location */
    *task->stack_ptr = ((usys) (task->stack_ptr)) + 20 ;

    /* Now set the stack pointer to eflags which is another 16 bytes */
    /* below.                                                        */

    task->stack_ptr -= 3 ;
 
} /* End of function cpu_task_ram_init() */

