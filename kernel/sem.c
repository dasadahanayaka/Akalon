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
/* File Name       : sem.c                                                   */
/* Description     : Akalon Semaphore functions                              */
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
/* Function Name   : sem_new                                                 */
/* Description     : Create a new Semaphore                                  */
/* Notes           :                                                         */
/*                                                                           */
/*---------------------------------------------------------------------------*/
usys     sem_new (usys count_max, usys count_start, char *name, usys *id)
{
    sem_t *new_sem ;

    /* Error Checking */
    if (count_start > count_max)  
       return E_VALUE ;

    /* Allocate a Semaphore Block  */
    if ((new_sem = malloc (sizeof (sem_t))) == NULL)
       return E_MEM ;

    /* Now start filling out the semBlk */
    new_sem->id        = (usys) new_sem ;
    new_sem->count_now = count_start ;
    new_sem->count_max = count_max   ;

    strncpy (new_sem->name, name, NAME_SIZE) ;

    new_sem->wait_q = NULL ;

    /* Link the new Semaphore to the Semaphore queue. */
    os_pause() ;

    new_sem->next  = kernel.sems ;
    kernel.sems    = new_sem ;

    if (kernel.int_mode == NO) 
       os_restart() ;

    /* Return back the ID...  */
    *id = new_sem->id ; 

    return GOOD ;

} /* End of function sem_new () */



/*---------------------------------------------------------------------------*/
/*                                                                           */
/* Function Name   : sem_get                                                 */
/* Description     : Gets a semaphore                                        */
/* Notes           : decrement count                                         */
/*                                                                           */
/*---------------------------------------------------------------------------*/
usys     sem_get (usys id, usys time_out)
{
    tcb_t  *this_task  ;
    usys   stat = GOOD ;
    sem_t  *sem ;

    sem = (sem_t *) id ;
    if (sem->id != id)
       return E_ID ;

    cpu_disable_ints() ;

    if (sem->count_now != 0)
    { 
       /* When the count is not zero, this task can continue  */
       /* to execute. Just subtract the count.                */
       sem->count_now -- ;

    } else {

       /* The count is zero, which means the semaphore is not */
       /* available. The task needs to wait or return back if */
       /* there is no timeout specified.                      */

       if (time_out == DONT_WAIT)
       {
          stat = E_RES ;
	  goto err ;
       } 

       /* Prepare this task for waiting for the semaphore  */
       this_task = (tcb_t *) task_id_get() ;

       this_task->wait_id = id ;    /* semaphore id */
       this_task->got_resource = NO ;

       /* Prepare for a time out condition...*/
       if (time_out != WAIT_FOREVER)
       {
	  /* Activate the Timer */
          this_task->time_count   = time_out ;
          this_task->timer_active = YES ;
       } 

       /* Mark this task as TASK_WAITING and run the next task.    */
       this_task->state = TASK_WAITING ;
       task_run_next (this_task) ;

       /* We have returned. This could be due to the fact that the */
       /* semaphore was received or a timeout occured awaiting the */
       /* semaphore. If the semaphore was not recieved but the     */
       /* timeout occured, notify the user of the timeout.         */

       if ((this_task->got_resource == NO) && (this_task->time_out == YES))
          stat = E_TIMEOUT ;
    }
    
err:
    if (kernel.int_mode == NO)
       cpu_enable_ints() ;

    return stat ;

} /* End of function sem_get() */



/*---------------------------------------------------------------------------*/
/*                                                                           */
/* Function Name   : sem_give                                                */
/* Description     : Gives a semaphore                                       */
/* Notes           : Increment count_now !!                                  */
/*                                                                           */
/*---------------------------------------------------------------------------*/
usys     sem_give (usys id)
{
    tcb_t *waiting_task ;
    tcb_t *this_task ;
    sem_t *sem ;

    sem = (sem_t *) id ;

    if (sem->id != id)
       return E_ID;

    cpu_disable_ints() ;

    /* When giving a semaphore, count_now is incremented by 1. */
    if (sem->count_max != sem->count_now)
    {
       sem->count_now ++ ;
    } else {

       if (kernel.int_mode == NO)
          cpu_enable_ints() ;

       return BAD ;
    }

    /* If count_now is 1, there could be a task waiting for this */
    /* semaphore. Therefore, check if a task is waiting, and if  */
    /* there is a waiting task, make it ready. If the task that  */
    /* was waiting is at a higher priority, then stop running    */
    /* this task and switch to that task.                        */

    if (sem->count_now == 1)
    { 
       /* Check if a higher priority task is waiting for this semaphore. */ 
       this_task = (tcb_t *) task_id_get() ;

       if (task_awaiting (id, &waiting_task) == YES)
       {  /* YES...A task is waiting for the semaphore.  */
          
          /* Change it to a TASK_READY state */
	  waiting_task->got_resource = YES ;
          waiting_task->state        = TASK_READY ;
          waiting_task->timer_active = NO ;
          waiting_task->time_out     = NO ;

	  /* ...and reduce the count_now value */
	  sem->count_now -- ;

	  /* Is this task at a higher priority than the calling task ? */
          if (this_task->priority > waiting_task->priority)
	  {  
             /* YES, The waiting task is at a higher priority. Need to */
             /* switch to that task...*/
             this_task->state = TASK_READY ;
             task_run_next (this_task) ;
          } 
       } 
    }

    if (kernel.int_mode == NO)
       cpu_enable_ints() ;

    return GOOD ;
} /* End of function sem_give() */



/*---------------------------------------------------------------------------*/
/*                                                                           */
/* Function Name   : sem_info                                                */
/* Description     : Provides information on a semaphore                     */
/* Notes           : Can query only according to id.                         */
/*                                                                           */
/*---------------------------------------------------------------------------*/
usys     sem_info (usys id, usys *count_max, usys *count_now, char *name)
{
    sem_t *sem ;

    sem = (sem_t *) (id) ;

    if (sem->id != id)
       return E_ID ;

    *count_now = sem->count_now ;
    *count_max = sem->count_max ;

    strncpy (name, sem->name, NAME_SIZE) ;

    return GOOD ;

} /* End of function sem_info() */



/*---------------------------------------------------------------------------*/
/*                                                                           */
/* Function Name   : sem_del                                                 */
/* Description     : Removes a semaphore from the system                     */
/* Notes           :                                                         */
/*                                                                           */
/*---------------------------------------------------------------------------*/
usys     sem_del (usys id)
{
    sem_t *sem ;

    sem = (sem_t *) id ;

    if (sem->id != id)
       return E_ID ;

    return GOOD ;

} /* End of function sem_del () */



/*---------------------------------------------------------------------------*/
/*                                                                           */
/* Function Name   : sem_init                                                */
/* Description     : Initialize the Semaphore Unit.                          */
/* Notes           :                                                         */
/*                                                                           */
/*---------------------------------------------------------------------------*/
usys     sem_init (void)
{
    kernel.sems = NULL ;
    return GOOD ;
} /* End of function sem_init() */



