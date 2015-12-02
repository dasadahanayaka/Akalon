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

#include "raspberry-pi.h"

extern   usys      bss_start[], bss_end[] ;
extern   link_t    uart_link ;


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

    /* Initialize the Kernel...*/

    stat = os_init ((usys) bss_end, BSP_MEM_TOP) ; 

    /* os_init () will only return on an error. */

    printf ("Failed to Initialize Kernel. Status = %x\n", stat) ;
    while (1) ;

} /* End of function bsp_pre_init () */



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

    /* The irq vector (loc 0x8018) and the fiq vector (0x801c) got  */
    /* wiped out by the function cpu_int_init(). Therefore, replace */
    /* them. These values originally came from the linker when file */
    /* boot.s was compiled. For some other reason, these values has */
    /* to be written to 0x18 and 0x1c instead of 0x8018 and 0x801c. */

    vec_loc = (volatile u32 *) 0x18 ;
    *vec_loc = 0xe59ff018 ;

    vec_loc = (volatile u32 *) 0x1c ;
    *vec_loc = 0xe59ff018 ;

    /* Initialize the rest of the system */

    if (dev_init() != GOOD)
      printf ("ERR: dev_init() failed !!!\n") ;

    if (stdio_init () != GOOD)
       printf ("ERR: stdio_init() Failed !!!\n") ; 

    if (cli_init () != GOOD)
       printf ("ERR: cli_init() Failed !!!\n") ;

    /* Module Links */
    os_link (&stdio_link, &uart_link, &uart_link) ;

} /* End of function bsp_post_init () */


extern   void      test_init(int, int) ;
void     do_not_call_this_func (void)
{
    test_init(1,0x40000) ;
} 

