/*---------------------------------------------------------------------------*/
/* Akalon RTOS                                                               */
/* Copyright (c) 2011-2016, Dasa Dahanayaka                                  */
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
/* File Name       : task_api.c                                              */
/* Description     : Akalon Task API functions                               */
/* Notes           :                                                         */
/*                                                                           */
/*---------------------------------------------------------------------------*/
#include <akalon.h>
#include <string.h>
#include <stdlib.h>

#include "kernel.h"
#include "cpu.h"



/*---------------------------------------------------------------------------*/
/*                                                                           */
/* Function Name   : task_new                                                */
/* Description     : Creates a task                                          */
/* Notes           :                                                         */
/*                                                                           */
/*---------------------------------------------------------------------------*/
usys   task_new    (usys priority, usys time_slice, usys stack_size,
                    usys max_msgs, usys max_msg_size, void (*entry_func)(),
                    usys arg0, usys arg1, usys arg2, char *name, usys *id)
{
    tcb_t *this_task, *new_task  ;
    usys size ;

    /* Get ram for the TCB + CPU stack + user stack */
    size = sizeof (tcb_t) + stack_size + cpu_task_ram_size() ;

    new_task = malloc (size) ;
    if (new_task == NULL)
       return E_MEM ;

    /* Initialize the whole area (i.e. tcb + cpu stack + user stack */
    memset (new_task, 0, size) ;

    /* Now start filling out the TCB */
    new_task->id           = (usys) new_task ;
    new_task->priority     = priority     ;
    new_task->stack_size   = stack_size   ;
    new_task->time_slice   = time_slice   ;
    new_task->max_msgs     = max_msgs     ;
    new_task->max_msg_size = max_msg_size ;
    new_task->func         = entry_func   ;

    strncpy (new_task->name, name, NAME_SIZE) ;

    new_task->timer_active = NO           ;
    new_task->time_count   = 0            ;
    new_task->state        = TASK_READY   ;
    new_task->wait_id      = 0            ;

    /* Initialize the Message Box */
    if (msgbox_init (max_msgs, max_msg_size, &new_task->msg_box) != GOOD)
       return BAD ;  /* More stuff <-- DO */

    /* Temproraly, set the stack pointer (i.e stack_ptr) to the end */
    /* of the TCB. The processor specific code will move it to the  */
    /* correct point. This is because, depending on the processor,  */
    /* the stack can go up or down.                                 */

    new_task->stack_ptr = (usys *) ((usys) new_task + sizeof (tcb_t)) ;

    /* Call the processor specific code to initialize the stack   */
    /* and register information.                                  */

    cpu_task_ram_init (new_task, arg0, arg1, arg2) ;

    /* Fill out the return Information */
    *id = new_task->id ;

    os_pause() ;

    /* Now Queue the TCB to the task queue.... */
    task_add_to_q (new_task) ;

    /* If the calling task is not known, then this is the first */
    /* task. In this case, just return back */
    if (task_id_get() == 0)
       return GOOD ;

    /* Check if the calling task needs to be switched to the just   */
    /* created task, becuase the just created task has a higher     */
    /* priority. ??                                                 */

    this_task = (tcb_t *) task_id_get() ;

    if (this_task->priority > new_task->priority)
    {
       this_task->state = TASK_READY ;
       task_run_next (this_task) ;
    }

    /* Enable interrupts only if this is not the Interrupt Task     */
    if (kernel.int_mode == NO)
       os_restart() ;

    return GOOD ;

} /* End of function task_new () */



/*---------------------------------------------------------------------------*/
/*                                                                           */
/* Function Name   : task_delete                                             */
/* Description     : Removes a Task from the system                          */
/* Notes           :                                                         */
/*                                                                           */
/*---------------------------------------------------------------------------*/
usys     task_delete (usys id)
{
    tcb_t *tmp_task  ;

    tmp_task = (tcb_t *) (id) ;

    if (tmp_task->id != id)
       return E_ID ;

    /* Make sure the calling task is not the one getting deleted */
    if (id == task_id_get())
       return BAD ;

    /* Mark the task for deletion. The kernel_task() will delete it */
    tmp_task->state = TASK_DELETE ;

    return GOOD ;
} /* End of function task_delete () */



/*---------------------------------------------------------------------------*/
/*                                                                           */
/* Function Name   : task_delay                                              */
/* Description     : Delay the currently running task.                       */
/* Notes           :                                                         */
/*                                                                           */
/*---------------------------------------------------------------------------*/
void     task_delay (usys tics)
{
    tcb_t *this_task ;

    os_pause() ;

    this_task = (tcb_t *) task_id_get() ;

    this_task->time_count   = tics ;
    this_task->time_out     = NO   ;
    this_task->state        = TASK_WAITING ;
    this_task->timer_active = YES  ;
    task_run_next (this_task) ;

    os_restart() ;

} /* End of function task_delay () */



/*---------------------------------------------------------------------------*/
/*                                                                           */
/* Function Name   : task_info                                               */
/* Description     : Provides information on an existing task.               */
/* Notes           : Finds a task by it's ID.                                */
/*                                                                           */
/*---------------------------------------------------------------------------*/
usys     task_info (usys id, usys *priority, usys *time_slice,
                    usys *stack_size, usys *max_msgs, usys *max_msg_size,
                    char *name)
{
    tcb_t *tmp_task ;

    tmp_task = (tcb_t *) id ;

    if (tmp_task->id != id)
       return E_ID ;

    *priority     = tmp_task->priority     ;
    *stack_size   = tmp_task->stack_size   ;
    *time_slice   = tmp_task->time_slice   ;
    *max_msgs     = tmp_task->max_msgs     ;
    *max_msg_size = tmp_task->max_msg_size ;

    strncpy (name, tmp_task->name, NAME_SIZE) ;

    return GOOD  ;

} /* End of function task_info () */



/*---------------------------------------------------------------------------*/
/*                                                                           */
/* Function Name   : task_sleep                                              */
/* Description     : Puts a Task to Sleep                                    */
/* Notes           :                                                         */
/*                                                                           */
/*---------------------------------------------------------------------------*/
void     task_sleep (void)
{
    tcb_t *this_task = (tcb_t *) task_id_get() ;

    cpu_disable_ints() ;

    this_task->timer_active = NO ;
    this_task->state = TASK_SLEEPING ;

    task_run_next (this_task) ;

    cpu_enable_ints() ;

} /* End of function task_sleep() */



/*---------------------------------------------------------------------------*/
/*                                                                           */
/* Function Name   : task_wake                                               */
/* Description     : Wake up a sleeping task                                 */
/* Notes           :                                                         */
/*                                                                           */
/*---------------------------------------------------------------------------*/
usys     task_wake (usys task_id)
{
    tcb_t *this_task, *task_to_wake ;

    /* Check if the task_id is valid */
    task_to_wake = (tcb_t *) task_id ;
    if (task_to_wake->id != task_id)
       return E_ID ;

    /* There is no point waking self, because it's already running */
    this_task = (tcb_t *) task_id_get() ;

    if (this_task->id == task_to_wake->id)
       return E_ID ;

    if (task_to_wake->state != TASK_SLEEPING) 
       return BAD  ;

    /* Wake the task by making it ready */
    task_to_wake->state = TASK_READY ;

    /* If the calling task (i.e this_task) is the interrupt task, then   */
    /* no need to switch. But if it's not the interrupt task, then check */
    /* if the woken task is at a higher priority. If the woken task is   */
    /* at a higher priority, then switch to it.                          */

    if (kernel.int_mode == NO)
       if (this_task->priority > task_to_wake->priority)
       {
          this_task->state = TASK_READY ;
          task_run_next (this_task) ;
       }

    return GOOD ;

} /* End of function task_wake() */


