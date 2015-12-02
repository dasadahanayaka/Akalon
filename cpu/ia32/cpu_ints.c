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
/* File Name       : cpu_ints.c                                              */
/* Description     : Akalon Interrupt functions specific to the IA32         */
/* Notes           :                                                         */
/*                                                                           */
/*---------------------------------------------------------------------------*/
#include <akalon.h>
#include "../../kernel/kernel.h"
#include "../../kernel/cpu.h"

#include "cpu_ia32.h"

idtLocType idtLoc ;



/*---------------------------------------------------------------------------*/
/*                                                                           */
/* Function Name   : setIDT                                                  */
/* Description     : Sets an interrupt related info in the IDT               */
/* Notes           :                                                         */
/*                                                                           */
/*---------------------------------------------------------------------------*/
void     setIDT  (usys intNum, void (*ISR)())
{
    GDTDesType  *des  ;

    /* Set the IDT Information */
    des = (GDTDesType *) (intNum * 8) ;

    des->offsetLow  = (u16) ( ((usys) ISR) & 0x0000ffff) ;
    des->offsetHigh = (u16) ( ((usys) ISR) >> 16       ) ;

    des->segSelect  = 0x0008 ;
    des->dWordCnt   = 0x00   ;
    des->access     = 0x8e   ;

} /* End of function setIDT() */


/*---------------------------------------------------------------------------*/
/*                                                                           */
/* Function Name   : cpu_init_init                                           */
/* Description     : Initializes the CPU's interrupt unit.                   */
/* Notes           :                                                         */
/*                                                                           */
/*---------------------------------------------------------------------------*/
usys     cpu_int_init (void)
{
    usys i ;

    /* Initialize all the interrupts to the null ISR */
    for(i = 0; i < 256; i++)
       setIDT (i, nullISR) ;

    /* Start connecting interrupts. Min OK for now <-- DO */
    setIDT (0x20,int32ISR) ;  
    setIDT (0x21,int33ISR) ;  
    setIDT (0x24,int36ISR) ;  

    idtLoc.limit = 0x7ff ;
    idtLoc.base  = 0x0   ;

    loadIDT () ;

    return GOOD  ;

} /* End of function cpu_int_init() */





