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
/* File Name       : test_init.c                                             */
/* Description     : Kernel Test for Akalon                                  */
/* Notes           :                                                         */
/*                                                                           */
/*---------------------------------------------------------------------------*/
#include <akalon.h>
#include <stdio.h>

extern   void      test_core   (void) ;
extern   void      test_speed  (usys arg) ;
extern   void      test_timers (usys arg) ;
extern   void      test_msgs   (usys arg) ;
extern   void      test_events (usys arg) ;

static   usys      task1_id  ;
static   usys      task2_id  ;
static   usys      sem1_id   ;
static   usys      timer1_id ;


/*---------------------------------------------------------------------------*/
/*                                                                           */
/* Function Name   : timer_func                                              */
/* Description     : Entry Point for the Timer                               */
/* Notes           : This is called from an interrupt context !!             */
/*                                                                           */
/*---------------------------------------------------------------------------*/
void     timer_func (void)
{
    sem_give (sem1_id) ;
} /* End of function timer_func() */



/*---------------------------------------------------------------------------*/
/*                                                                           */
/* Function Name   : task1                                                   */
/* Description     : Entry Point for Task 1.                                 */
/* Notes           : At a higher priority than Task 2                        */
/*                                                                           */
/*---------------------------------------------------------------------------*/
static   void     task1 (void)
{
    u8    in_box [32] ;
    usys  src_id ;
    usys  size ;
    usys  stat ;

    while (1)
    {
       if ((stat = msg_get (&src_id, &size, in_box, WAIT_FOREVER)) != GOOD)
           printf ("ERR: msg_get Failed !!! Task 1 Stat %d\n", stat) ;

       printf ("Task 1: Got MSG\n") ;
    }

#if 0
      task_delay (200) ;
#endif

} /* End of Function task1() */



/*---------------------------------------------------------------------------*/
/*                                                                           */
/* Function Name   : task2                                                   */
/* Description     : Entry Point for Task 2.                                 */
/* Notes           : Is at a lower priority than Task 1                      */
/*                                                                           */
/*---------------------------------------------------------------------------*/
static   void     task2 (void)
{
    u8     out_box [32] ;
    usys   stat ;

    while (1)
    {
       if ((stat = sem_get (sem1_id, WAIT_FOREVER)) != GOOD)
          printf ("ERR: sem_get() Failed !!! Sem 1. Stat %d\n", stat) ;

       printf ("Task 2: Got Sem\n") ;

       /* Send msg to task 1 */

       if ((stat = msg_send (task1_id, 11, out_box)) != GOOD)
          printf ("ERR: msg_send() Failed !!! Stat %d\n", stat) ;
    }

#if 0
       task_delay (50) ;
#endif

} /* End of Function task2() */



/*---------------------------------------------------------------------------*/
/*                                                                           */
/* Function Name   : test_start                                              */
/* Description     : Test the Kernel !!!                                     */
/* Notes           :                                                         */
/*                                                                           */
/*---------------------------------------------------------------------------*/
usys     test_start (usys test_num, usys arg)
{
    usys    stat  ;

    switch (test_num)
    {
       case 0:
	    test_core () ;  
	    return GOOD ;

       case 1:
	    test_speed (arg) ;  
	    return GOOD ;

       case 2:
	    test_timers (arg) ;  
	    return GOOD ;

       case 3:
	    test_msgs (arg) ;  
	    return GOOD ;

       case 4:
	    test_events (arg) ;  
	    return GOOD ;

       default:
            break ;
    }

    printf("Creating Task 1\n") ;

    stat = task_new (50, 0, 1024, 3, 32, task1, 0,0,0, "task1", &task1_id) ;
    if (stat == GOOD)
    {
       printf("Created Task 1 (ID = %x)\n", task1_id) ;
    } else {
       printf("ERR: (TEST) Creating task1. Stat %d\n", stat) ;
       return BAD ;
    }

    printf("Creating Task 2\n") ;

    stat = task_new (51, 0, 1024, 3, 32, task2, 0,0,0, "task2", &task2_id) ;
    if (stat == GOOD)
    {
       printf("Created Task 2 (ID = %x)\n", task2_id) ;
    } else {
       printf("ERR: (TEST) Creating task2. Stat %d\n", stat) ;
       return BAD ;
    }

    /* Create the Semaphore (Binary/Empty) */

    if ((stat = sem_new (1, 0, "Test Sem", &sem1_id)) == GOOD)
    {
       printf("Created Sem 1 (ID = %x)\n", sem1_id) ;
    } else {
       printf("ERR: (TEST) Creating sem1_id. Stat %d\n", stat) ;
       return BAD ;
    }

    /* Create the Timer */

    stat = timer_new (10, TIMER_REPEAT, timer_func, "Test Timer",
                      &timer1_id) ;
    if (stat == GOOD)
    {
       printf ("Created Timer 1 (ID = %x)\n", timer1_id) ;
    } else {
       printf ("ERR: (TEST) Creating timer1_id. Stat %d\n", stat) ;
       return BAD ;
    }

    /* Start the Timer */

    if ((stat = timer_start (timer1_id)) == GOOD)
    {
       printf ("Started Timer 1\n", timer1_id) ;
    } else {
       printf ("ERR: (TEST) Starting timer1_id. Stat %d\n", stat) ;
       return BAD ;
    }

    return GOOD ;

} /* End of function test_start() */


/*---------------------------------------------------------------------------*/
/*                                                                           */
/* Function Name   : test_init                                               */
/* Description     : Test initialization routine                             */
/* Notes           :                                                         */
/*                                                                           */
/*---------------------------------------------------------------------------*/
usys     test_init  (void)
{
    return GOOD ;
} /* End of function test_init() */
