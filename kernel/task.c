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
/* File Name       : task.c                                                  */
/* Description     : Akalon non-API Tasks functions                          */
/* Notes           :                                                         */
/*                                                                           */
/*---------------------------------------------------------------------------*/
#include <akalon.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>

#include "kernel.h"
#include "cpu.h"


/*---------------------------------------------------------------------------*/
/*                                                                           */
/* Function Name   : task_id_get                                             */
/* Description     : Returns the ID of task that called this function        */
/*                                                                           */
/* Notes           : The implementation is simple. Get the address of "addr" */
/*                   which is on the stack and locate the 2 consecative task */
/*                   IDs (which are tcb addresses) that surround it. The     */
/*                   stack address belongs to the lower task ID.             */
/*                                                                           */
/*---------------------------------------------------------------------------*/
usys     task_id_get (void)
{  
    usys   addr, size ;
    tcb_t  *task ;

    size = sizeof (tcb_t) + cpu_task_ram_size() ;
    task = kernel.task_q ;

    while (task != NULL)
    {
       if (( ((usys) &addr) >= task->id) &&
           ( ((usys) &addr) < (task->id + size + task->stack_size)))
	  return (task->id) ;
   
       task = task->nBlk ;
    }

    return (0) ;
} /* End of function task_id_get() */



/*---------------------------------------------------------------------------*/
/*                                                                           */
/* Function Name   : task_ready_get                                          */
/* Description     : Provides the next ready task                            */
/*                                                                           */
/* Notes           : This function can be called from the interrupt vector   */
/*                   routine int_run().                                      */
/*                                                                           */
/*                   WARNING: Call this function with interrupts disabled !! */
/*                                                                           */
/*                   NOTE: This function needs to be fast because the task   */
/*                         scheduler depends on it !!!                       */
/*                                                                           */
/*---------------------------------------------------------------------------*/
tcb_t    *task_ready_get (void)
{
    tcb_t *task_tmp ;

    /* The first part of this function (upto the end of the while() */
    /* loop) should do the least amount because of performance.     */

    task_tmp = kernel.task_q ;

    /* This while() loop should do the minimum of stuff... */
    while (task_tmp != NULL)
    {
       if (task_tmp->state == TASK_READY) 
	  return task_tmp ;

       task_tmp = task_tmp->nBlk ;
    }

    /* Cannot find a runnable task. Therefore return the idle task. */
    return (kernel.task_idle) ;

} /* End of function task_ready_get () */



/*---------------------------------------------------------------------------*/
/*                                                                           */
/* Function Name   : task_run_next                                           */
/* Description     : Run the next ready task...                              */
/*                                                                           */
/* Notes           : Called when a higher priority task is ready to run.     */
/*                                                                           */
/*                   WARNING: Call this function with interrupts disabled !! */
/*---------------------------------------------------------------------------*/
void     task_run_next (tcb_t *calling_task)
{
    tcb_t *task_next ;

    /* Run the next highest priority task */
    task_next = task_ready_get () ;

    task_next->state = TASK_RUNNING ;
    cpu_task_switch (&calling_task->stack_ptr, task_next->stack_ptr) ;

} /* End of function task_run_next() */



/*---------------------------------------------------------------------------*/
/*                                                                           */
/* Function Name   : task_start                                              */
/* Description     : Entry point for all Tasks                               */
/* Notes           :                                                         */
/*                                                                           */
/*---------------------------------------------------------------------------*/
void     task_start (void (*func)(), usys arg0, usys arg1, usys arg2)
{
    /* Do not enable interrupts when the idle task is first run */
    if (kernel.task_idle->func != func) 
       cpu_enable_ints() ;

    /* Call the Caller's Entry Point */
    func (arg0, arg1, arg2) ;

    /* Handle a task exit <-- DO */
    while (1)
      printf ("ERR: You Cannot Exit the Task !!!\n") ;

} /* End of function task_start() */



/*---------------------------------------------------------------------------*/
/*                                                                           */
/* Function Name   : task_init                                               */
/* Description     : Initialize the Task Unit.                               */
/* Notes           :                                                         */
/*                                                                           */
/*---------------------------------------------------------------------------*/
usys     task_init (void)
{
    /* Initialize the kernel_struct structure */

    kernel.task_q     = NULL ;
    kernel.task_idle  = NULL ;

    return GOOD ;
} /* End of function task_init() */



/*---------------------------------------------------------------------------*/
/*                                                                           */
/* Function Name   : task_awaiting                                           */
/* Description     : Return back the highest priority task awaiting for a    */
/*                   certian resource.                                       */
/* Notes           :                                                         */
/*                                                                           */
/*          WARNING: Call this function with interrupts disabled !!!         */
/*---------------------------------------------------------------------------*/
usys     task_awaiting (usys resource, tcb_t **task)
{
    tcb_t *tmp_task ;

    tmp_task = kernel.task_q ;

    while (tmp_task != NULL)
    {
       if ((tmp_task->state == TASK_WAITING) && 
           (tmp_task->wait_id == resource)) 
       {
	  *task = tmp_task ;
	  return YES ;
       }

       tmp_task = tmp_task->nBlk ;
    }

    *task = NULL ;
    return NO ;

} /* End of function task_awaiting () */



/*---------------------------------------------------------------------------*/
/*                                                                           */
/* Function Name   : save_task_state                                         */
/* Description     : After an interrupt, the interrupted task's state is     */
/*                   saved into it's TSB. This function does this job.       */
/* Notes           :                                                         */
/*                                                                           */
/*---------------------------------------------------------------------------*/
void     save_task_state (usys *stack_ptr)
{
    tcb_t *this_task = (tcb_t *) task_id_get() ;

    this_task->stack_ptr = stack_ptr  ;

} /* End of function save_task_state() */



/*---------------------------------------------------------------------------*/
/*                                                                           */
/* Function Name   : t                                                       */
/* Description     : Prints all the tasks in the system                      */
/* Notes           :                                                         */
/*                                                                           */
/*---------------------------------------------------------------------------*/
void     t (void)
{
    tcb_t *task ;

    printf ("\nname id priority stack state\n") ;

    task = kernel.task_q ;

    while (task != NULL)
    {
       printf("%s ", task->name) ;
       printf("0x%x ", task->id) ;
       printf("%d ", task->priority) ;
       printf ("0x%x ",(usys) task->stack_ptr) ;
       switch (task->state)
       {
          case 1 :
	       printf ("Runnning") ;
	       break ;  
          case 2 :
	       printf ("Spinning") ;
	       break ;  
          case 3 :
	       printf ("Ready")    ;
	       break ;  
          case 4 :
	       printf ("Waiting")  ;
	       break ;
          default:  
	       printf ("Unknown 0x%x", task->state)  ;
       }

       printf ("\n") ;

       task = task->nBlk ;
    }

} /* End of function t() */



/*---------------------------------------------------------------------------*/
/*                                                                           */
/* Function Name   : task_add_to_q                                           */
/* Description     : Queue a TCB to the Task Q.                              */
/* Notes           :                                                         */
/*                                                                           */
/*          WARNING: Call this function with interrupts disabled !!!         */
/*---------------------------------------------------------------------------*/
void     task_add_to_q (tcb_t *task)
{
    tcb_t *pTCB ; /* Previous TCB */
    tcb_t *cTCB ; /* Current  TCB */

    /* First check if the head is empty. This is a very common occurence, */
    /* therefore its ok to make it a special case.                        */

    if (kernel.task_q == NULL)
    {
       task->pBlk    = NULL ;
       task->nBlk    = NULL ;
       kernel.task_q = task ;
       return ;
    }

    pTCB = NULL  ;
    cTCB = kernel.task_q ;

    for(;;)
    {
        /* Check if the tcb can be inserted between 2 TCBs. */

        if (cTCB->priority > task->priority)
        {  /* and yes it can !!! */

           task->nBlk = cTCB ;
           cTCB->pBlk = task ;

           if (pTCB != NULL)
           {  /* Insertion not to the Front!  */

              pTCB->nBlk = task ;
              task->pBlk = pTCB ;

           } else {
              /* Insertion to the Front. */

              task->pBlk     = NULL ;
              kernel.task_q = task ;
           }

           return ;
        }

        /* Check if can be instered at the end of the queue. */

        if (cTCB->nBlk == NULL)
        {
           task->pBlk = cTCB ;
           task->nBlk = NULL ;
           cTCB->nBlk = task ;
           return ;
        }

        /* Cannot insert. Loop back and test  */
        /* the next TCB.                      */

        pTCB = cTCB ;
        cTCB = cTCB->nBlk ;

    } /* forever loop */

} /* End of function task_add_to_q() */



/*---------------------------------------------------------------------------*/
/*                                                                           */
/* Function Name   : task_rm_from_q                                          */
/* Description     : Remove a task TCB from the Task Q                       */
/* Notes           :                                                         */
/*                                                                           */
/*          WARNING: Call this function with interrupts disabled !!!         */
/*---------------------------------------------------------------------------*/
void     task_rm_from_q (tcb_t *task)
{
    /* Check if the task is at the head of the queue...*/

    if (task->pBlk == NULL)
    {
       /* This is the first item on the queue...*/

       kernel.task_q = task->nBlk ;

       /* Check if there was a valid next pointer. */
       /* If there was, make it point to NULL      */

       if (task->nBlk != NULL)
          task->nBlk->pBlk = NULL ;

    } else {

       /* This is not the first item on the queue. Fix double */
       /* link list.                                          */

       /* Update the previous pointer's next pointer to point */
       /* to the next pointer.                                */

       task->pBlk->nBlk = task->nBlk ;

       /* Now check if there was a next pointer. If there was */
       /* then make it point to the previous pointer.         */

       if (task->nBlk != NULL)
         task->nBlk->pBlk = task->pBlk ;

    }

    task->nBlk = NULL ;
    task->pBlk = NULL ;

} /* End of function task_rm_from_q() */



