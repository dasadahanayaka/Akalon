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
/* Description     : BSP Entry point for a PC                                */
/* Notes           :                                                         */
/*                                                                           */
/*---------------------------------------------------------------------------*/
#include <akalon.h>
#include <stdio.h>
#include "pc.h"


char   *help_table[] =
{
    "c         -- Show Copyright",
    "h         -- Show Help Menu",
    "r         -- Read Address",
    "t         -- Show Clock Tics",
    "v         -- Show Version",
    "w         -- Write Address",
    "test_init -- Test Kernel",
    "cls       -- Clear Screen",
    "in8       -- Read  IO (8 Bits)",
    "in16      -- Read  IO (16 Bits)",
    "in32      -- Read  IO (32 Bits)",
    "out8      -- Write IO (8 Bits)",
    "out16     -- Write IO (16 Bits)",
    "out32     -- Write IO (32 Bits)",
    "pci_show  -- Show all PCI Devices",
    "pci_dev   -- Show a PCI Device",
    NULL
} ;



/*---------------------------------------------------------------------------*/
/*                                                                           */
/* Function Name   : bsp_pre_init                                            */
/* Description     : Entry point before the kernel is initialized.           */
/* Notes           :                                                         */
/*                                                                           */
/*---------------------------------------------------------------------------*/
void     bsp_pre_init (usys heap_addr, usys heap_size) 
{
    usys  stat ;

    /* Clear the Screen. This initializes dbg_putchar() */
    cls () ;

    /* Remap the Interrupts */
    remapInts() ;

    printf ("Initializing Akalon...\n") ;

    stat = os_init (heap_addr, heap_size) ;

    /* os_init () will only return on an error. */
    printf ("Failed to Initialize Kernel. Status = %x\n", stat) ;

    while (1) ;

} /* End of function bsp_pre_init() */



/*---------------------------------------------------------------------------*/
/*                                                                           */
/* Function Name   : bsp_post_init                                           */
/* Description     : Entry point after the Kernel is initialized.            */
/* Notes           : Called by the Idle Task.                                */
/*                                                                           */
/*---------------------------------------------------------------------------*/
void     bsp_post_init (void)
{
    /***********************/
    /* Initialize all Apps */
    /***********************/

    if (B8000_init () != GOOD)
       printf ("ERR: B8000_init() Failed !!!\n") ; 

    if (stdio_init () != GOOD)
       printf ("ERR: initStdio() Failed !!!\n") ; 

    if (cli_init () != GOOD)
       printf ("ERR: cli_init() Failed !!!\n") ; 

    /**************************/
    /* Initialize all Devices */
    /**************************/

    dev_init() ;

    /* Link all the Stackable Applications. This can happen only */
    /* after the Applications and the Devices are initialized.   */

#if 1 /* To interact via the serial port, change this to a 0 */
    os_link (&stdio_link, &kbd_link, &b8000_link) ;
#else
    os_link (&stdio_link, &ns16550_link, &ns16550_link) ;
#endif

    /* Put User Application Entry Function here */

    /* Enable the PIC so interrupts can get to the processor */
    enablePcInts () ;

    return ; 

} /* End of function bsp_post_init () */


/*---------------------------------------------------------------------------*/
/*                                                                           */
/* Function Name   : h                                                       */
/* Description     : Show Help                                               */
/* Notes           :                                                         */
/*                                                                           */
/*---------------------------------------------------------------------------*/
void     h (void)
{
    char **item ;

    printf ("\nHelp Menu\n") ;

    item = help_table ;

    while (*item != NULL)
    {
       printf ("%s\n", *item) ;
       item++ ;
    }

} /* End of function help() */


extern   void      test_init() ;
void     do_no_call_this_func (void)
{
    test_init () ;
}



