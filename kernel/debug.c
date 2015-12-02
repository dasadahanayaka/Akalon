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
/* File Name       : debug.c                                                 */
/* Description     : Akalon Debug functions                                  */
/* Notes           :                                                         */
/*                                                                           */
/*---------------------------------------------------------------------------*/
#include <akalon.h>
#include <stdio.h>

#include "kernel.h"


/*---------------------------------------------------------------------------*/
/*                                                                           */
/* Function Name   : dbg_ps                                                  */
/* Description     : Print's the current stack.                              */
/* Notes           :                                                         */
/*                                                                           */
/*---------------------------------------------------------------------------*/
void     dbg_ps (usys *stack)
{
     usys *tmp = NULL, i ;
     tcb_t *task ;

     task = (tcb_t *) task_id_get() ;
     printf ("Task id is 0x%x\n", task->id) ;

     tmp  = stack ;
     tmp += 20 ;

     printf ("Stack PTR = 0x%x\n", (usys) stack) ;

     for (i=0; i < 23; i++)
     {
        printf ("[0x%x] 0x%x\n", (usys) tmp, *tmp) ;
	tmp-- ;
     }
     printf ("\n") ;

#if 0
     while (1) ;
#endif

} /* End of function dbg_ps() */






