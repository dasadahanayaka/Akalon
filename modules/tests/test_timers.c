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
/* File Name       : test_timers.c                                           */
/* Description     : Akalon Timer Test                                       */
/* Notes           :                                                         */
/*                                                                           */
/*---------------------------------------------------------------------------*/
#include <akalon.h>
#include <stdio.h>
#include <string.h>

static   usys  sem ;
static   usys  t1_jif_old = 0 ;
static   usys  t2_jif_old = 0 ;


/*---------------------------------------------------------------------------*/
/*                                                                           */
/* Function Name   : task1                                                   */
/* Description     : Entry Point for Task 1.                                 */
/* Notes           : At a higher priority than Task 2                        */
/*                                                                           */
/*---------------------------------------------------------------------------*/
static   void      task1 (void)
{
    usys stat, jif_new, diff ;

    while (1)
    {
       if ((stat = sem_get (sem, 75)) == E_TIMEOUT)
       {
	  jif_new = jiffy ;
          diff = jif_new - t1_jif_old ;

          if (diff == 75)
	  {
             printf ("Task 1: sem_get() timeout %d\n", diff) ;
	  } else {
	     if (t1_jif_old != 0)
                printf ("ERR: Task 1: Timeout was %d\n", diff) ;
	  }

          t1_jif_old = jif_new ;
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
    usys stat, jif_new, diff ;

    while (1)
    {
       if ((stat = sem_get (sem, 50)) == E_TIMEOUT)
       {
	  jif_new = jiffy ; 
          diff = jif_new - t2_jif_old ;

          if (diff == 50)
	  {
             printf ("Task 2: sem_get() timeout %d\n", diff) ;
	  } else {
	     if (t2_jif_old != 0)
                printf ("ERR: Task 1: Timeout was %d\n", diff) ;
	  }

          t2_jif_old = jif_new ;
       } else {
          printf ("ERR: Task 2. sem_get(). Stat %d\n", stat) ;
       }
    }

} /* End of Function task2() */


/*---------------------------------------------------------------------------*/
/*                                                                           */
/* Function Name   : task3                                                   */
/* Description     : Entry Point for Task 3.                                 */
/* Notes           : Is at a lower priority than Task 2                      */
/*                                                                           */
/*---------------------------------------------------------------------------*/
static   void      task3 (void)
{
    usys old_jiffy ;
    usys new_jiffy ;

    while (1) 
    {
       old_jiffy = jiffy ;
       task_delay (100)  ;
       new_jiffy = jiffy ; 

       if ((new_jiffy - old_jiffy) == 100)
       {
          printf ("Task3: Diff is 100\n") ; 
       } else {
          printf ("ERR: Task3 Diff is %d\n", new_jiffy - old_jiffy) ; 
       }   
    }
} /* End of function task3() */



/*---------------------------------------------------------------------------*/
/*                                                                           */
/* Function Name   : test_timers                                             */
/* Description     : Test Akalon Timers                                      */
/* Notes           :                                                         */
/*                                                                           */
/*---------------------------------------------------------------------------*/
usys     test_timers (usys arg)
{
    usys task_id, stat ; 

    printf("Creating Task 1\n") ;

    stat = task_new (50,0,1024,1,32,task1,0,0,0,"task1", &task_id) ;
    if (stat == GOOD)
    {
       printf("Created Task 1 (ID = %x)\n", task_id) ;
    } else {
       printf("ERR: (TEST) Creating Task 2. Stat %d\n", stat) ;
       return BAD ;
    }

    printf("Creating Task 2\n") ;

    stat = task_new (51,0,1024,1,32,task2,0,0,0,"task2", &task_id) ;
    if (stat == GOOD)
    {
       printf("Created Task 2 (ID = %x)\n", task_id) ;
    } else {
       printf("ERR: (TEST) Creating Task 2. Stat %d\n", stat) ;
       return BAD ;
    }

    printf("Creating Task 3\n") ;

    stat = task_new (53,0,1024,1,32,task3,0,0,0,"task3", &task_id) ;
    if (stat == GOOD)
    {
       printf("Created Task 3 (ID = %x)\n", task_id) ;
    } else {
       printf("ERR: (TEST) Creating Task 3. Stat %d\n", stat) ;
       return BAD ;
    }

    if ((stat = sem_new (1, 0, "Test Sem", &sem)) == GOOD)
    {
       printf("Created Sem (ID = %x)\n", sem) ;
    } else {
       printf("ERR: (TEST) Creating Semaphore. Stat %d\n", stat) ;
       return BAD ;
    }

    return GOOD ;
} /* End of function test_timers() */


