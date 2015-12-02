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
/* Description     : Akalon Interrupt functions specific to the ARM          */
/* Notes           :                                                         */
/*                                                                           */
/*---------------------------------------------------------------------------*/
#include <akalon.h>
#include <string.h>
#include "../../kernel/kernel.h"
#include "../../kernel/cpu.h"

#include "cpu_arm.h"

usys     irq_stack [2] ;

/*---------------------------------------------------------------------------*/
/*                                                                           */
/* Function Name   : cpu_init_init                                           */
/* Description     : Initializes the CPU's interrupt unit.                   */
/* Notes           :                                                         */
/*                                                                           */
/*---------------------------------------------------------------------------*/
usys     cpu_int_init (void)
{
    /* Programming the branch instruction for the IRQ vector */
    /* (offset 0x18) and the FIQ vector (offset 0x1c).       */

    memcpy ((void *) 0x18, (void *) irq_vector, 4) ;
    memcpy ((void *) 0x1c, (void *) fiq_vector, 4) ;

    irq_mode_init (irq_stack) ;

    return GOOD  ;

} /* End of function cpu_int_init() */





