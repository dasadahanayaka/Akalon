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
/* File Name       : config.c                                                */
/* Description     : Configuration Code                                      */
/* Notes           :                                                         */
/*                                                                           */
/*---------------------------------------------------------------------------*/
#include <akalon.h>
#include <stdio.h>
#include <config.h>

/*---------------------------------------------------------------------------*/
/*                                                                           */
/* Function Name   : mod_init                                                */
/* Description     : Configure all the modules.                              */
/* Notes           : Only module writers should modify this function         */
/*                                                                           */
/*---------------------------------------------------------------------------*/
usys     mod_init (void)
{
    usys stat = GOOD ;

#ifdef   INCLUDE_STDIO
    if (stdio_init () != GOOD)
    {
       printf ("ERR: initStdio() Failed !!!\n") ;
       stat = BAD ; 
    }
#endif
#ifdef   INCLUDE_CLI
    if (cli_init () != GOOD)
    {
       printf ("ERR: cli_init() Failed !!!\n") ; 
       stat = BAD ; 
    }
#endif
#ifdef   INCLUDE_TESTS
    if (test_init () != GOOD)
    {
       printf ("ERR: test_init() Failed !!!\n") ; 
       stat = BAD ; 
    }
#endif

    return stat ;
} /* End of function mod_init () */


