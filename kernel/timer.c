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
/* File Name       : timer.c                                                 */
/* Description     : Akalon Timer functions                                  */
/* Notes           :                                                         */
/*                                                                           */
/*---------------------------------------------------------------------------*/
#include <akalon.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>

#include "kernel.h"


usys     jiffy   ;


/*---------------------------------------------------------------------------*/
/*                                                                           */
/* Function Name : timer_new                                                 */
/* Description   : Creates a new timer                                       */
/* Notes         :                                                           */
/*                                                                           */
/*---------------------------------------------------------------------------*/
usys     timer_new (usys tics, usys type, void (*func)(), char *name,
                    usys *id)
{
    timer_t *new_timer ;

    /* Alloc space for timer */
    if ((new_timer = malloc (sizeof (timer_t))) == NULL)
       return E_MEM ;

    /* Fill out the structure */
    new_timer->id         = (usys) new_timer ;
    new_timer->tics       = tics ;
    new_timer->type       = type ;
    new_timer->func       = func ;
    strncpy (new_timer->name, name, NAME_SIZE) ;

    new_timer->active     = NO   ;
    new_timer->tics_now   = 0    ;

    /* Link the Timer to the Timer List. */
       
    os_pause() ;

    new_timer->next = kernel.timers ;
    kernel.timers   = new_timer ;

    if (kernel.int_mode == NO)
       os_restart() ;

    /* Return back the ID */
    *id = new_timer->id ;

    return GOOD ;

} /* End of function timer_new() */



/*---------------------------------------------------------------------------*/
/*                                                                           */
/* Function Name : timer_delete                                              */
/* Description   : Remove a Timer                                            */
/* Notes         :                                                           */
/*                                                                           */
/*---------------------------------------------------------------------------*/
usys     timer_delete (usys id)
{
    timer_t *timer, *tmp, *prev ;

    timer = (timer_t *) id ;

    if ((timer->id != id) || (kernel.timers == NULL))
       return E_ID ;

    os_pause() ;

    /* Find the Previous Timer Control Block */

    prev = NULL ;
    tmp  = kernel.timers ;

    while (tmp != NULL)
    {
       if (tmp == timer)
	  break ;

       prev = tmp ;
       tmp  = tmp->next ;
    }

    if (prev == NULL)
    {  /* First timer on List */
       kernel.timers = timer->next ;
    } else {
       /* Not the First timer on list */
       prev->next = timer->next ;
    }

    /* Free the timer blk */
    timer->id = 0 ;
    free (timer)  ;

    if (kernel.int_mode == NO)
       os_restart() ;

    return GOOD ;

} /* End of function timer_delete() */



/*---------------------------------------------------------------------------*/
/*                                                                           */
/* Function Name : timer_start                                               */
/* Description   : Start a timer                                             */
/* Notes         :                                                           */
/*                                                                           */
/*---------------------------------------------------------------------------*/
usys     timer_start (usys id)
{
    timer_t *timer ;

    timer = (timer_t *) id ;

    if (timer->id != id)
       return E_ID ;

    if (timer->tics_now == 0)
       timer->tics_now = timer->tics ;

    timer->active = YES ;

    return GOOD ;

} /* End of function timer_start() */



/*---------------------------------------------------------------------------*/
/*                                                                           */
/* Function Name : timer_stop                                                */
/* Description   : Stop a timer                                              */
/* Notes         :                                                           */
/*                                                                           */
/*---------------------------------------------------------------------------*/
usys     timer_stop (usys id)
{
    timer_t *timer ;

    timer = (timer_t *) id ;

    if (timer->id != id)
       return E_ID ;

    timer->active = NO ;

    return GOOD ;

} /* End of function timer_stop() */



/*---------------------------------------------------------------------------*/
/*                                                                           */
/* Function Name : timer_info                                                */
/* Description   : Provides information on a timer                           */
/* Notes         :                                                           */
/*                                                                           */
/*---------------------------------------------------------------------------*/
usys     timer_info (usys id, usys *tics, usys *type, char *name,
                     usys *tics_now, usys *is_active)
{
    timer_t *timer ;

    timer = (timer_t *) id ;

    if (timer->id != id)
       return E_ID ;

    *tics = timer->tics ;
    *type = timer->type ;
    strncpy (name, timer->name, NAME_SIZE) ;
    *tics_now   = timer->tics_now ; 
    *is_active  = timer->active ;

    return GOOD ;

} /* End of function timer_info() */



/*---------------------------------------------------------------------------*/
/*                                                                           */
/* Function Name : timer_tic                                                 */
/* Description   : Notifies the kernel of a timer tic                        */
/* Notes         : Called from the interrupt context                         */
/*                                                                           */
/*---------------------------------------------------------------------------*/
void     timer_tic (void)
{
    tcb_t   *task  ;
    timer_t *timer ;

    jiffy ++ ;

    /**************************/
    /* First parse the Timers */
    /**************************/

    timer = kernel.timers ;

    while (timer != NULL)
    {
        /* First check if the timer is active */ 
        if (timer->active == NO)
	{  /* Timer not active...*/
	   timer = timer->next ;
	   continue ;
	}

        timer->tics_now -- ;

        if (timer->tics_now == 0)
        {
           /* See if the timer needs to be marked as in-active */

           if (timer->type == TIMER_ONESHOT)
	   {  /* Mark timer as in-active */
	      timer->active = NO ;
           } else {
              /* Reset the time count */
              timer->tics_now = timer->tics ;
           }

           /* Call the timer function */
           timer->func() ;
        }

        /* Continue to next timer */
        timer = timer->next ;
    }

    /***************************/
    /* Now parse all the tasks */
    /***************************/

    task = kernel.task_q ;

    while (task != NULL)
    {
       if (task->timer_active == YES)
       {
          task->time_count -- ;

          /* Check if the task's timer has popped */
          if (task->time_count == 0)
          {
             task->timer_active = NO ;

             /* Timer has pop'ed. Transition the task from the "waiting" */
             /* to "ready" state. Also indicate the timeout.             */

             task->time_out = YES ;
             task->state = TASK_READY ;
	  }
       }

       task = task->nBlk ;
    }

} /* End of function timer_tic() */


/*---------------------------------------------------------------------------*/
/*                                                                           */
/* Function Name : timer_init                                                */
/* Description   : Initialize the Timer Unit.                                */
/* Notes         :                                                           */
/*                                                                           */
/*---------------------------------------------------------------------------*/
usys     timer_init (void)
{
    kernel.timers = NULL ;
    kernel.tics_per_sec = STD_TICS_PER_SEC ;
    jiffy = 0 ;

    return GOOD ;

} /* End of function timer_init () */



/*---------------------------------------------------------------------------*/
/*                                                                           */
/* Function Name : timer_tics_per_sec                                        */
/* Description   : Return the amount of tics per second                      */
/* Notes         :                                                           */
/*                                                                           */
/*---------------------------------------------------------------------------*/
usys     timer_tics_per_sec (void)
{
    return kernel.tics_per_sec ;
} /* End of function timer_tics_per_sec () */


/*---------------------------------------------------------------------------*/
/*                                                                           */
/* Function Name   : tics                                                    */
/* Description     : Print the Tics                                          */
/* Notes           :                                                         */
/*                                                                           */
/*---------------------------------------------------------------------------*/
void     tics (void)
{
    printf ("\n-> %d", jiffy) ;
} /* End of function tics () */
