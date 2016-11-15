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
extern   void      test_tasks  (usys arg) ;



/*---------------------------------------------------------------------------*/
/*                                                                           */
/* Function Name   : test_full                                               */
/* Description     : Full test of Akalon                                     */
/* Notes           :                                                         */
/*                                                                           */
/*---------------------------------------------------------------------------*/
usys     test_full (usys test_num, usys arg)
{
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
	    test_tasks (arg) ;
	    return GOOD ;

       default:
            break ;
    }

    return GOOD ;

} /* End of function test_full() */


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
