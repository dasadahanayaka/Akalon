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
/* File Name       : test_events.c                                           */
/* Description     : Akalon Event Test                                       */
/* Notes           :                                                         */
/*                                                                           */
/*---------------------------------------------------------------------------*/
#include <akalon.h>
#include <stdio.h>
#include <string.h>


static   usys  task1_id ;
static   usys  task2_id ;
static   usys  sem_id   ;





/*---------------------------------------------------------------------------*/
/*                                                                           */
/* Function Name   : task1                                                   */
/* Description     : Entry Point for Task 1.                                 */
/* Notes           : At a higher priority than Task 2                        */
/*                                                                           */
/*---------------------------------------------------------------------------*/
static   void      task1 (void)
{
    usys stat ;

    while (1)
    {
       if ((stat = sem_get (sem_id, WAIT_FOREVER)) == GOOD)
       {
          printf ("Task 1: sem_get() Returned !!!\n") ;
       } else {
          printf ("ERR: Task 1 sem_get(). Stat %d\n", stat) ;
       }
    }

} /* End of Function task1() */



/*---------------------------------------------------------------------------*/
/*                                                                           */
/* Function Name   : task2                                                   */
/* Description     : Entry Point for Task 2.                                 */
/* Notes           : Is at a lower priority than Task 1                      */
/*                                                                           */
/*---------------------------------------------------------------------------*/
static   void      task2 (void)
{
    usys stat ;

    while (1)
    {
       task_sleep() ; 

       if ((stat = sem_give (sem_id)) != GOOD)
          printf ("ERR: Task 2. sem_get(). Stat %d\n", stat) ;  
    }

} /* End of Function task2() */



/*---------------------------------------------------------------------------*/
/*                                                                           */
/* Function Name   : timer_func                                              */
/* Description     : Entry Point for the Timer                               */
/* Notes           : This is called from an interrupt context !!             */
/*                                                                           */
/*---------------------------------------------------------------------------*/
static   void     timer_func (void)
{
    task_wake (task2_id) ;
} /* End of function timer_func() */



/*---------------------------------------------------------------------------*/
/*                                                                           */
/* Function Name   : test_events                                             */
/* Description     : Test Akalon Events                                      */
/* Notes           :                                                         */
/*                                                                           */
/*---------------------------------------------------------------------------*/
usys     test_events (usys arg)
{
    usys timer_id, stat ; 

    printf("Creating Task 1\n") ;

    stat = task_new (50,0,1024,1,32,task1,0,0,0,"task1", &task1_id) ;
    if (stat == GOOD)
    {
       printf("Created Task 1 (ID = %x)\n", task1_id) ;
    } else {
       printf("ERR: (TEST) Creating Task 2. Stat %d\n", stat) ;
       return BAD ;
    }

    printf("Creating Task 2\n") ;

    stat = task_new (51,0,1024,1,32,task2,0,0,0,"task2", &task2_id) ;
    if (stat == GOOD)
    {
       printf("Created Task 2 (ID = %x)\n", task2_id) ;
    } else {
       printf("ERR: (TEST) Creating Task 2. Stat %d\n", stat) ;
       return BAD ;
    }

    if ((stat = sem_new (1, 0, "Test Sem", &sem_id)) == GOOD)
    {
       printf("Created Sem (ID = %x)\n", sem_id) ;
    } else {
       printf("ERR: (TEST) Creating Semaphore. Stat %d\n", stat) ;
       return BAD ;
    }

    /* Create the Timer */
    stat = timer_new (10, TIMER_REPEAT, timer_func, "Test Timer",
                      &timer_id) ;
    if (stat == GOOD)
    {
       printf ("Created Timer (ID = %x)\n", timer_id) ;
    } else {
       printf ("ERR: (TEST) Creating timer1_id. Stat %d\n", stat) ;
       return BAD ;
    }

    /* Start the Timer */
    stat = timer_start (timer_id) ;
    if (stat == GOOD)
    {
       printf ("Started Timer (ID = %x)\n", timer_id) ;
    } else {
       printf ("ERR: (TEST) Creating timer1_id. Stat %d\n", stat) ;
       return BAD ;
    }

    return GOOD ;

} /* End of function test_events() */


