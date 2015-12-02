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
/* File Name       : test_speed.c                                            */
/* Description     : Akalon Speed Test                                       */
/*                                                                           */
/* Notes           : The Algorithm is...                                     */
/*                                                                           */
/*                   a. Create Task1 at a higher priority than Task2         */
/*                   b. Create a binary empty semaphore                      */
/*                   c. Task1 waits for the semaphore                        */
/*                   d. Task2 releases the semaphore which makes Task1 run   */
/*                   e. Task1 waits for the semaphore again (step c)         */
/*                   f. Task2 releases the semaphore again (step d)          */
/*                                                                           */
/*---------------------------------------------------------------------------*/
#include <akalon.h>
#include <stdio.h>
#include <string.h>

static   usys      sem ;


/*---------------------------------------------------------------------------*/
/*                                                                           */
/* Function Name   : task1                                                   */
/* Description     : Entry Point for Task 1.                                 */
/* Notes           : At a higher priority than Task 2                        */
/*                                                                           */
/*---------------------------------------------------------------------------*/
static   void      task1 (usys print)
{
    usys old_jiffy ;
    usys stat, count = 0 ;

    old_jiffy = jiffy ;

    while (1)
    {
       if ((stat = sem_get (sem, WAIT_FOREVER)) != GOOD)
       {
          printf ("ERR: Task 1 sem_get(). Stat %d\n", stat) ;
	  continue ;
       }

       if (!(++count % print))
       { 
          printf ("Task 1: count = %d Time %d tics\n", count,
                  jiffy - old_jiffy) ;
          old_jiffy = jiffy ;
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
    usys   stat ;

    while (1)
    {
       /* Start off by giving the semaphore */
       if ((stat = sem_give (sem)) != GOOD)
          printf ("ERR: Task 2 sem_give(). Stat %d\n", stat) ;
    }

} /* End of Function task2() */



/*---------------------------------------------------------------------------*/
/*                                                                           */
/* Function Name   : test_speed                                              */
/* Description     : Test the speed of the OS                                */
/* Notes           :                                                         */
/*                                                                           */
/*---------------------------------------------------------------------------*/
usys     test_speed (usys arg)
{
    usys task_id, stat ; 

    printf("Creating Task 1\n") ;

    stat = task_new (50,0,1024,1,32,task1,arg,0,0,"task1", &task_id) ;
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

    if ((stat = sem_new (1, 0, "Test Sem", &sem)) == GOOD)
    {
       printf("Created Sem (ID = %x)\n", sem) ;
    } else {
       printf("ERR: (TEST) Creating Semaphore. Stat %d\n", stat) ;
       return BAD ;
    }

    return GOOD ;
} /* End of function test_speed() */


