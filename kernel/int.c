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
/* File Name       : int.c                                                   */
/* Description     : Akalon Interrupt functions                              */
/* Notes           :                                                         */
/*                                                                           */
/*---------------------------------------------------------------------------*/
#include <akalon.h>
#include "kernel.h"
#include "cpu.h"


/*---------------------------------------------------------------------------*/
/*                                                                           */
/* Function Name   : int_config                                              */
/* Description     : Configure an interrupt                                  */
/* Notes           :                                                         */
/*                                                                           */
/*---------------------------------------------------------------------------*/
usys     int_config (usys int_num, usys status, void (*isr)())
{
    if (int_num > kernel.max_ints)
       return E_VALUE ;

    kernel.usr_isr [int_num] = isr ;
    return GOOD ;

} /* End of function int_config () */



/*---------------------------------------------------------------------------*/
/*                                                                           */
/* Function Name   : int_run                                                 */
/* Description     : Run the User ISR                                        */
/*                                                                           */
/* Notes           : Called from the Interrupt Vector Code.                  */
/*                                                                           */
/*---------------------------------------------------------------------------*/
usys     int_run (int int_num)
{
    tcb_t  *this_task, *next_task ;

    /* First, call the user registered ISR */
    kernel.int_mode = YES ;

    if (kernel.usr_isr [int_num] != NULL)
       kernel.usr_isr [int_num] () ;

    kernel.int_mode = NO ;

    /* Now figure out if another higher priority task became */
    /* ready as a result of running the above ISR. We send   */
    /* back this information to the interrupt vector code so */
    /* it knows if to do a task switch or not.               */

    /* First get this task's id so it's state can be changed */
    this_task = (tcb_t *) task_id_get() ;

    /* Change the state to ready so task_ready_get() works.  */
    this_task->state = TASK_READY ;
    next_task = task_ready_get () ;

    if (this_task->priority > next_task->priority)
    {
       next_task->state = TASK_RUNNING ;
       return ((usys) next_task->stack_ptr) ;
    }

    this_task->state = TASK_RUNNING ;
    return 0 ;
} /* End of function int_run() */



/*---------------------------------------------------------------------------*/
/*                                                                           */
/* Function Name   : int_init                                                */
/* Description     : Initializes the Interrupt Unit.                         */
/* Notes           :                                                         */
/*                                                                           */
/*---------------------------------------------------------------------------*/
usys     int_init (void)
{
    /* Get the Maximum number of cpu interrupts */
    kernel.max_ints = CPU_MAX_INTS ;

    kernel.int_num = 0 ;
    cpu_int_init () ;
    
    return GOOD ;

} /* End of function save_int_init() */


