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
/* File Name       : mem.c                                                   */
/* Description     : Akalon Memory Management functions                      */
/* Notes           :                                                         */
/*                                                                           */
/*---------------------------------------------------------------------------*/
#include <akalon.h>
#include <string.h>

#include "kernel.h"



/*---------------------------------------------------------------------------*/
/*                                                                           */
/* Function Name   : malloc                                                  */
/* Description     :                                                         */
/* Notes           : Simple as it gets. Need to impliment a "real" malloc    */
/*                                                                           */
/*---------------------------------------------------------------------------*/
void     *malloc (size_t size)
{
    void *loc  ;

    /* Error Checking...*/
    if ((size == 0) || (size > kernel.mem_free_size))
       return NULL ;

    loc = (void *) kernel.mem_free_loc ;

    /* Make sure all locations are aligned on the processor's native */
    /* size (i.e sizeof(usys)). This can be accomplished by setting  */
    /* the size to be aligned on a processor native boundary         */
    
    if (size % sizeof (usys))
       size += sizeof (usys) - (size % sizeof (usys)) ;

    kernel.mem_free_loc  += size ;
    kernel.mem_free_size -= size ;

#if 0 /* Enable this to make sure there are no uninitialized variables */
    memset (loc,0xff,size) ;
#endif

    return loc ;   
} /* End of function malloc() */



/*---------------------------------------------------------------------------*/
/*                                                                           */
/* Function Name   : free                                                    */
/* Description     :                                                         */
/* Notes           : Currently not implimented                               */
/*                                                                           */
/*---------------------------------------------------------------------------*/
void     free (void *ptr)
{
} /* End of function free() */



/*---------------------------------------------------------------------------*/
/*                                                                           */
/* Function Name   : mem_init                                                */
/* Description     : Initialize the Memory Unit                              */
/* Notes           :                                                         */
/*                                                                           */
/*---------------------------------------------------------------------------*/
usys     mem_init (usys mem_start, usys size)
{
    usys delta ;

    /* Error Checking */
    if ((mem_start == 0) || (size == 0))
       return BAD ;

    /* Make sure Memory starts on a processor native boundary */
    if (mem_start % sizeof (usys))
    {
       delta = sizeof (usys) - (mem_start % sizeof (usys)) ;

       mem_start += delta ;
       size      -= delta ;
    }

    kernel.mem_start = kernel.mem_free_loc  = mem_start ;
    kernel.mem_size  = kernel.mem_free_size = size ;

    return GOOD ;

} /* End of function mem_init () */


