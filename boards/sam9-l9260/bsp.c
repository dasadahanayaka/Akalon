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
    volatile u32 *vec_loc ;

    /* In the case of an irq interrupt, the CPU starts executing from */
    /* address 0x18 (i.e. irq_vector()), which has to load or branch  */
    /* to the code that handles the interrupt (i.e. irq_isr()). This  */
    /* all gets setup during kernel initialization (i.e os_init())    */ 
    /* via the cpu_int_init() call.                                   */
    /*                                                                */
    /* Since this BSP loads @ address 0x2000_0000, the irq_vector()   */
    /* code points to the irq_isr() code that is located somwhere in  */
    /* the 0x2XXX_XXXX range. However, the ARM processor cannot do a  */
    /* branch that is greater than 32MB, which is a problem since the */
    /* interrupt vector is located at 0x18 and the irq_isr() will be  */
    /* located in the 0x2XXX_XXXX range.                              */
    /*                                                                */
    /* Fortunetly, the at91sam9260 processor has a register located   */
    /* at 0xffff_f100 which is accessible from 0x18 (albiet from the  */
    /* negative offset of 0xf20) and is also updated with the address */
    /* of the isr corrosponding to the active interrupt.              */
    /*                                                                */
    /* Thus, we write the opcode value of 0xe51f_ff20, whose value    */
    /* found by dumping the object code.                              */

    vec_loc = (volatile u32 *) 0x18 ;
    *vec_loc = 0xe51fff20 ;

    if (dev_init() != GOOD)
       printf ("ERR: dev_init() failed !!!\n") ;

    /* Initialize the configured modules */
    mod_init() ;

    /*** Module Links ***/

    /* stdio */
    os_link (&stdio_link, &uart_link, &uart_link) ;

    /* network */
    os_link (&net_link, &emac_link, &emac_link) ;

} /* End of function bsp_post_init () */

