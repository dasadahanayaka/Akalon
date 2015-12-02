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
/* File Name       : bsp.c                                                   */
/* Description     : BSP Code                                                */
/* Notes           :                                                         */
/*                                                                           */
/*---------------------------------------------------------------------------*/
#include <akalon.h>
#include <stdio.h>

#include "sam9_l9260.h"

extern   link_t uart_link ;


/*---------------------------------------------------------------------------*/
/*                                                                           */
/* Function Name   : bsp_post_init                                           */
/* Description     : Entry point after the Kernel is initialized.            */
/* Notes           : Called by the Idle Task.                                */
/*                                                                           */
/*---------------------------------------------------------------------------*/
void     bsp_post_init (void)
{
    if (dev_init() != GOOD)
      printf ("ERR: dev_init() failed !!!\n") ;

    if (stdio_init () != GOOD)
       printf ("ERR: initStdio() Failed !!!\n") ; 

    if (cli_init () != GOOD)
       printf ("ERR: cli_init() Failed !!!\n") ;

    /* Module Links */
    os_link (&stdio_link, &uart_link, &uart_link) ;

} /* End of function bsp_post_init () */


extern   void      test_init() ;
void     do_not_call_this_func (void)
{
    test_init() ;
} 

