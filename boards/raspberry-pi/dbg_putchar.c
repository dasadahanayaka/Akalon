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
/* File Name       : dbg_putchar.c                                           */
/* Description     : The Debug Output for the Raspberry Pi                   */
/* Notes           : Da BSP started here !!!                                 */
/*                                                                           */
/*---------------------------------------------------------------------------*/
#include <akalon.h>


/*---------------------------------------------------------------------------*/
/*                                                                           */
/* Function Name   : dbg_putchar                                             */
/* Description     : Puts a character onto the screen                        */
/* Notes           : Only use in Debug Mode                                  */
/*                                                                           */
/*---------------------------------------------------------------------------*/
void     dbg_putchar (int c)
{
    volatile u32 *addr ;

    addr = (volatile u32 *) 0x20201018 ;

    /* Poll for TXFF Bit */
    while (*addr & 0x20) ;

    addr = (volatile u32 *) 0x20201000 ;

    *addr = (char) c ;

} /* End of function dbg_putchar() */



