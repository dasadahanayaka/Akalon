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
/* File Name       : kernel.c                                                */
/* Description     : The Akalon Kernel Initialization.                       */
/* Notes           :                                                         */
/*                                                                           */
/*---------------------------------------------------------------------------*/
#include <akalon.h>
#include <stdio.h> 
#include <string.h>

#include "kernel.h"
#include "cpu.h"


kernel_t kernel ;


/*---------------------------------------------------------------------------*/
/*                                                                           */
/* Function Name   : task_idle                                               */
/* Description     : The Idle Task                                           */
/*                                                                           */
/* Notes           : This is the first run task and the task that is run     */
/*                   when there is no runnable tasks. The first time this    */
/*                   task is run, interrupts are disabled.                   */
/*---------------------------------------------------------------------------*/
void     task_idle (void)
{
    tcb_t *task ;

    printf ("Akalon: In Idle Task\n") ;

    /* We are in the Idle Task. Now let the BSP create the rest of  */
    /* the System's resources (tasks, sems, init routines, etc) by  */
    /* calling bsp_post_init(). However, interrupts should never be */
    /* enabled in bsp_post_init(). Also, note that any new task     */
    /* that's created in bsp_post_init() will not switch out the    */
    /* Idle Task because it is running at the highest priority.     */

    bsp_post_init () ;

    kernel.int_mode = NO ; /* Remove this HACK !!! <--- DO */

    /* All System Resources have now been created. Lower the Idle   */
    /* Task's priority to the lowest.                               */

    task_rm_from_q (kernel.task_idle) ;
    kernel.task_idle->priority = -1   ;
    task_add_to_q  (kernel.task_idle) ;

    /* Get the highest priority task and run it. Interrupts will be */
    /* enabled when the task is run by task_start().                */

    task = task_ready_get() ;

    if (task != kernel.task_idle)
    {
       kernel.task_idle->state = TASK_READY ;
       task_run_next (kernel.task_idle) ;
    }

    /* Interrupts were never enabled when the idle task was run. So */
    /* enable interrupts and start spinning until another task gets */
    /* into the ready state, which can happen becuase an interrupt. */
    /* If an interrupt never happens (for what ever reason), then   */
    /* to code will be stuck in the following while(1) loop.        */   

    cpu_enable_ints() ;
    while (1) 
    {
       /* Check if any task needs to be deleted */
       task_cleanup() ;
    }
    
} /* End of function task_idle() */



/*---------------------------------------------------------------------------*/
/*                                                                           */
/* Function Name   : os_init                                                 */
/* Description     : Initialize the Kernel                                   */
/* Notes           :                                                         */
/*                                                                           */
/*---------------------------------------------------------------------------*/
usys     os_init (usys mem_start, usys mem_size)
{
    usys stat ;

    /* Disable all Interrupts */
    cpu_disable_ints() ;

    /* The global kernel data structre is statically allocated by   */
    /* the linker. This prevent the need for it to get allocated by */
    /* the Memory Unit which is itself not initialized.             */

    memset (&kernel, 0, sizeof (kernel_t)) ;

    /* Initialize the Memory Unit */
    if (mem_init (mem_start, mem_size) != GOOD)
    {
       printf ("ERR: In os_init(): mem_init() Failed !!!\n") ;
       return BAD ;
    }

    /* Initialize the Task Unit */
    if (task_init () != GOOD)
    {
       printf ("ERR: In os_init(): task_init () Failed !!!\n") ;
       return BAD ;
    }

    /* The memory and task units are now initialized. The kernel */
    /* task and the interrupt task can now be created. They are  */
    /* created with the highest priority.                        */

    kernel.int_mode = YES ; /* Fix this HACK <-- */

    printf ("Akalon: Creating Idle Task\n") ;

    stat = task_new (0, 0, 1024,0,0, task_idle, 0,0,0, "idle_task",
                     (usys *) (void *) &kernel.task_idle) ;
    if (stat != GOOD)
    { 
       printf ("ERR: In os_init(): Creating Idle Task. Stat = %d\n", stat) ;
       return BAD ;
    }

    /* Initialize the Interrupt Unit */
    if (int_init () != GOOD)
    {
       printf ("ERR: In os_init(): int_init () Failed !!!\n") ;
       return BAD ;
    }

    /* Initialize the Semaphore Unit */
    if (sem_init () != GOOD)
    {
       printf ("ERR: In os_init(): sem_init() Failed !!!\n") ;
       return BAD ;
    }

    /* Initialize the Timer Unit */
    if (timer_init () != GOOD)
    {
       printf ("ERR: In os_init(): timer_init() Failed !!!\n") ;
       return BAD ;
    }

    /* Now, run the Idle Task. Except in the case of */
    /* an Error, this call will never return back.   */

    kernel.task_idle->state = TASK_RUNNING ; 
    cpu_task_first (kernel.task_idle->stack_ptr) ;

    /* We did return back...something has gone wrong. */
    printf ("ERR: os_init(). Cannot Run Idle Task !!!\n") ;

    return BAD ;

} /* End of function os_init() */



/*---------------------------------------------------------------------------*/
/*                                                                           */
/* Function Name   : os_pause                                                */
/* Description     : Pause the kernel                                        */
/* Notes           :                                                         */
/*                                                                           */
/*---------------------------------------------------------------------------*/
void     os_pause (void)
{
    cpu_disable_ints() ;
} /* End of function os_pause() */



/*---------------------------------------------------------------------------*/
/*                                                                           */
/* Function Name   : os_restart                                              */
/* Description     : Fire back the kernel                                    */
/* Notes           :                                                         */
/*                                                                           */
/*---------------------------------------------------------------------------*/
void     os_restart (void)
{
    cpu_enable_ints() ;
} /* End of function os_restart() */
