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
/* File Name       : test_msgs.c                                             */
/* Description     : Akalon Message Test                                     */
/* Notes           :                                                         */
/*                                                                           */
/*---------------------------------------------------------------------------*/
#include <akalon.h>
#include <stdio.h>
#include <string.h>

static   usys  t1_id = 0 ;
static   usys  t2_id = 0 ;


/*---------------------------------------------------------------------------*/
/*                                                                           */
/* Function Name   : task1                                                   */
/* Description     : Entry Point for Task 1.                                 */
/* Notes           : At a higher priority than Task 2                        */
/*                                                                           */
/*---------------------------------------------------------------------------*/
static   void      task1 (void)
{
    usys stat, src_id, size ;
    u8   msg [32] ;

    while (1)
    {
       if ((stat = msg_get (&src_id, &size, msg, 15)) == GOOD)
       {
          printf ("Task1: Got Msg \"%s\" from taskId %d of size %d\n", 
                  msg, src_id, size) ; 
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
    u8   msg[32] = "Hello World" ;
    usys stat ;

    while (1)
    {
       if ((stat = msg_send (t1_id, 32, msg)) != GOOD)
          printf ("ERR: Task 2. msg_send(). Stat %d\n", stat) ;

       task_delay (50) ;
    }

} /* End of Function task2() */



/*---------------------------------------------------------------------------*/
/*                                                                           */
/* Function Name   : test_msgs                                               */
/* Description     : Test Akalon Messages                                    */
/* Notes           :                                                         */
/*                                                                           */
/*---------------------------------------------------------------------------*/
usys     test_msgs (usys arg)
{
    usys stat ; 

    printf("Creating Task 1\n") ;

    stat = task_new (50,0,1024,1,32,task1,0,0,0,"task1", &t1_id) ;
    if (stat == GOOD)
    {
       printf("Created Task 1 (ID = %x)\n", t1_id) ;
    } else {
       printf("ERR: (TEST) Creating Task 2. Stat %d\n", stat) ;
       return BAD ;
    }

    printf("Creating Task 2\n") ;

    stat = task_new (51,0,1024,1,32,task2,0,0,0,"task2", &t2_id) ;
    if (stat == GOOD)
    {
       printf("Created Task 2 (ID = %x)\n", t2_id) ;
    } else {
       printf("ERR: (TEST) Creating Task 2. Stat %d\n", stat) ;
       return BAD ;
    }

    return GOOD ;

} /* End of function test_msgs() */


