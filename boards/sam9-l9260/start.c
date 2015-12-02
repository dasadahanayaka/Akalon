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
/* File Name       : start.c                                                 */
/* Description     : BSP Entry Point for the SAM9-L9260 board.               */
/*                                                                           */
/* Notes           : Stoped working on this board after the clock and the    */
/*                   serial port was working.                                */
/*                                                                           */
/*---------------------------------------------------------------------------*/
#include <akalon.h>
#include <stdio.h>


extern   usys  bss_start[], bss_end[] ;


/*---------------------------------------------------------------------------*/
/*                                                                           */
/* Function Name   : bsp_pre_init                                            */
/* Description     : Entry point before the kernel is initialized.           */
/* Notes           :                                                         */
/*                                                                           */
/*---------------------------------------------------------------------------*/
void     bsp_pre_init (usys mem_start, usys mem_size)
{
    usys  *tmp, stat ;

    /* Initialize the BSS Section */
    tmp = bss_start ;

    while (tmp != bss_end)
      *tmp++ = 0 ;

    printf ("Initializing Akalon...\n") ;

    /* The SAM9-L9260 card has 64MB, starting at 0x20000000. Currently, */
    /* the Akalon RTOS for the SAM9-L9260 card is about 20K. However,   */
    /* 1 MB has been allocated for the kernel (i.e 0x20100000) which    */
    /* can then accomodate an application of about 980K. If the user    */
    /* application exceeds that, then change the following values, but  */
    /* make sure the starting address + free ram size doesn't exceed    */
    /* 0x24000000.                                                      */

    stat = os_init (0x20100000, 0x3efffff) ; 

    /* os_init () will only return on an error. */
    printf ("Failed to Initialize Kernel. Status = %x\n", stat) ;
    while (1) ;

} /* End of function bsp_pre_init () */


