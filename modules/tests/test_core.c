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
/* File Name       : test_core.c                                             */
/* Description     : Akalon basic functionality Test                         */
/* Notes           :                                                         */
/*                                                                           */
/*---------------------------------------------------------------------------*/
#include <akalon.h>
#include <stdio.h>
#include <string.h>

static   usys  t1_id, t2_id ;
static   usys  t1_count, t2_count ;
static   usys  sem_id, timer_id ;


/*---------------------------------------------------------------------------*/
/*                                                                           */
/* Function Name   : timer_func                                              */
/* Description     : Entry Point for the Timer                               */
/* Notes           : This is called from an interrupt context !!             */
/*                                                                           */
/*---------------------------------------------------------------------------*/
static   void      timer_func (void)
{
    t2_count = 0 ;
} /* End of function timer_func() */


/*---------------------------------------------------------------------------*/
/*                                                                           */
/* Function Name   : task1                                                   */
/* Description     : Entry Point for Task 1.                                 */
/* Notes           :                                                         */
/*                                                                           */
/*---------------------------------------------------------------------------*/
static   void      task1 (void)
{
    usys stat, src_id, size, jif_a, jif_b ;
    char msg [32] ;

    t1_count = 10 ;

    /* (1) Testing task_delay() */
    printf ("\nINF: Testing tasks...\n") ;

    jif_a = jiffy ;
    task_delay (15) ;
    jif_b = jiffy ;

    if (jif_b != jif_a + 15)
    {
       printf ("ERR: (1) task1() returned after %d tics !!!\n", jif_b) ;
       goto err ;
    }

    t1_count = 9 ;

    /* (2) Testing Semaphores */
    printf ("INF: Testing semaphores...\n") ;

    if ((stat = sem_get (sem_id, 300)) != GOOD)
    {
       printf ("ERR: (2) task1() sem_get failed !!!. Stat = %d\n", stat) ;
       goto err ;
    }

    t1_count = 8 ;

    /* (3) Testing messages  */
    printf ("INF: Testing messages...\n") ;

    if ((stat = msg_get (&src_id, &size, msg, 300)) != GOOD)
    {
       printf ("ERR: (3) task1() msg_get failed !!!. Stat = %d\n", stat) ;
       goto err ;
    }

    if ((src_id != t2_id) || (size != 32) ||
        (strcmp (msg, "Hello Akalon !!!") != 0))
    {
       printf ("ERR: (3) msg src, size or data was incorrect.\n") ;
       goto err ;
    }

    /* (3) Testing timers */
    printf ("INF: Testing timers...\n") ;

    task_delay (15) ;
    if (t2_count == 0)
       printf ("test_core() passed.\n") ;

    return ;

err:
    printf ("test_core() failed !!!. t1_count = %d t2_count = %d\n",
            t1_count, t2_count) ;

} /* End of Function task1() */



/*---------------------------------------------------------------------------*/
/*                                                                           */
/* Function Name   : task2                                                   */
/* Description     : Entry Point for Task 2.                                 */
/* Notes           :                                                         */
/*                                                                           */
/*---------------------------------------------------------------------------*/
static   void      task2 (void)
{
    usys stat, jif_a, jif_b ;
    u8   msg[32] = "Hello Akalon !!!" ;

    t2_count = 10 ;

    /* (1) Test the task_delay() function */

    jif_a = jiffy ;
    task_delay (20) ;
    jif_b = jiffy ;

    t2_count = 9 ;

    if (jif_b != jif_a + 20)
    {
       printf ("ERR: (1) task2() returned after %d tics.\n", jif_b ) ;
       return ;
    }

    t2_count = 8 ;

    /* (2) Test semaphores */

    if ((stat = sem_give (sem_id)) != GOOD)
    {
       printf ("ERR: (2) task2() sem_give failed. Stat = %d\n", stat ) ;
       return ;
    }

    t2_count = 7 ;

    /* (3) Test messages */

    if ((stat = msg_send (t1_id, 32, msg)) != GOOD)
    {
       printf ("ERR: (3) task2() msg_send() failed. Stat %d\n", stat) ;
       return ;
    }

    t2_count = 6 ;

    /* (4) Test timer */

    if ((stat = timer_start (timer_id)) != GOOD)
    {
       printf ("ERR: (4) task2() timer_start() filed. Stat %d\n", stat) ;
       return ;
    }

} /* End of Function task2() */



/*---------------------------------------------------------------------------*/
/*                                                                           */
/* Function Name   : test_core                                               */
/* Description     : Akalon's core test                                      */
/* Notes           :                                                         */
/*                                                                           */
/*---------------------------------------------------------------------------*/
usys     test_core (void)
{
    usys stat ;
    t1_count = t2_count = 0 ;

    printf("Creating Task 1...\n") ;

    stat = task_new (50,0,1024,1,32,task1,0,0,0,"task1", &t1_id) ;
    if (stat == GOOD)
    {
       printf("Created Task 1 (ID = %x)\n", t1_id) ;
    } else {
       printf("ERR: Can't create Task 1. Stat = %d\n", stat) ;
       return BAD ;
    }

    printf("Creating Task 2...\n") ;

    stat = task_new (51,0,1024,1,32,task2,0,0,0,"task2", &t2_id) ;
    if (stat == GOOD)
    {
       printf("Created Task 2 (ID = %x)\n", t2_id) ;
    } else {
       printf("ERR: Can't create Task 2. Stat = %d\n", stat) ;
       return BAD ;
    }

    printf("Creating Semaphore...\n") ;

    if ((stat = sem_new (1,0,"sem_core",&sem_id)) == GOOD)
    {
       printf("Created Semaphore (ID = %x)\n", sem_id) ;
    } else {
       printf("ERR: Can't create Semaphore. Stat = %d\n", stat) ;
       return BAD ;
    }

    printf("Creating Timer...\n") ;

    stat = timer_new (10, TIMER_REPEAT, timer_func, "Test Timer",
                      &timer_id) ;
    if (stat == GOOD)
    {
       printf ("Created Timer (ID = %x)\n", timer_id) ;
    } else {
       printf ("ERR: Can't create Timer. Stat = %d\n", stat) ;
       return BAD ;
    }

    return GOOD ;

} /* End of function test_core() */


