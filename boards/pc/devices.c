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
/* File Name       : devices.c                                               */
/* Description     : Initialize all the relavent devices                     */
/* Notes           :                                                         */
/*                                                                           */
/*---------------------------------------------------------------------------*/
#include <akalon.h>
#include <stdio.h>
#include "pc.h"



/*---------------------------------------------------------------------------*/
/*                                                                           */
/* Function Name   : io_write                                                */
/* Description     : Writes to an IO port                                    */
/* Notes           :                                                         */
/*                                                                           */
/*---------------------------------------------------------------------------*/
void     io_write (usys reg, usys val)
{
    out8 ((u16) reg, (u8) val) ;
} /* End of function io_write() */


/*---------------------------------------------------------------------------*/
/*                                                                           */
/* Function Name   : io_read                                                 */
/* Description     : Reads from an IO Port                                   */
/* Notes           :                                                         */
/*                                                                           */
/*---------------------------------------------------------------------------*/
usys     io_read (usys reg)
{
    return ((usys) in8 ((u16) reg)) ;
} /* End of function ioRead() */



/*---------------------------------------------------------------------------*/
/*                                                                           */
/* Function Name   : clock_isr                                               */
/* Description     :                                                         */
/* Notes           : Called by the interrupt task                            */
/*                                                                           */
/*---------------------------------------------------------------------------*/
void     clock_isr (void)
{
    timer_tic() ;
    sendEOI() ;
} /* End of Function clock_isr() */


/*---------------------------------------------------------------------------*/
/*                                                                           */
/* Function Name   : clk_init                                                */
/* Description     : Initialize the PC Clock.                                */
/* Notes           :                                                         */
/*                                                                           */
/*---------------------------------------------------------------------------*/
usys     clk_init (void)
{
    /* Set up the Clock ISR */
    int_config (0x20, ENABLE, clock_isr) ;

    return GOOD ;
} /* End of function clk_init () */



/*---------------------------------------------------------------------------*/
/*                                                                           */
/* Function Name   : serial_isr                                              */
/* Description     :                                                         */
/* Notes           : Called by the interrupt task                            */
/*                                                                           */
/*---------------------------------------------------------------------------*/
void     serial_isr (void)
{
    ns16550_isr() ;
    sendEOI() ;
} /* End of Function serial_isr() */



/*---------------------------------------------------------------------------*/
/*                                                                           */
/* Function Name   : serial_init                                             */
/* Description     : Initialize the PC Serial Port.                          */
/* Notes           :                                                         */
/*                                                                           */
/*---------------------------------------------------------------------------*/
usys     serial_init (void)
{
    ns16550_t dev  ;

    dev.reg_write = io_write ;
    dev.reg_read  = io_read  ;
    dev.base_addr = 0x3f8 ;

    dev.clk_freq  = 1843200  ;
    dev.baud_rate = 115200   ;
    dev.data_bits = 8 ;
    dev.parity    = PARITY_NONE ;
    dev.stop_bits = 1 ;

    /* Initialize the Device Driver */
    if (ns16550_init (&dev) != GOOD)
    {
       printf ("ERR: ns16550_init() Failed\n") ;
       return BAD ;
    }

    /* Set-up the Serial ISR */
    int_config (0x24, ENABLE, serial_isr) ;

    return GOOD ;

} /* End of Function serial_init() */



/*---------------------------------------------------------------------------*/
/*                                                                           */
/* Function Name   : pc_kbd_isr                                              */
/* Description     :                                                         */
/* Notes           :                                                         */
/*                                                                           */
/*---------------------------------------------------------------------------*/
void     pc_kbd_isr (void)
{
    u8   key ;

    /* Get key */
    key = readKeyPress () ;

    kbd_isr (key) ;
    sendEOI () ;

} /* End of Function pc_kbd_isr() */



/*---------------------------------------------------------------------------*/
/*                                                                           */
/* Function Name   : pc_kbd_init                                             */
/* Description     : Init routine for the PC Keyboard Module.                */
/* Notes           :                                                         */
/*                                                                           */
/*---------------------------------------------------------------------------*/
usys     pc_kbd_init (void)
{
    if (kbd_init () != GOOD)
    {
       printf ("ERR: kbd_init() Failed !!!\n") ;
       return BAD ;
    } 
 
    /* Set-up the Keyboard ISR */
    int_config (0x21, ENABLE, pc_kbd_isr) ;

    return GOOD ;

} /* End of Function pc_kbd_init () */


/*---------------------------------------------------------------------------*/
/*                                                                           */
/* Function Name   : pci_write                                               */
/* Description     : PCI Write Routine                                       */
/* Notes           : Might need a Semaphore                                  */
/*                                                                           */
/*---------------------------------------------------------------------------*/
void     pci_write (usys reg, usys val)
{
    out32 (0xcf8, reg) ;
    out32 (0xcfc, val) ;
} /* End of function pci_read() */



/*---------------------------------------------------------------------------*/
/*                                                                           */
/* Function Name   : pci_read                                                */
/* Description     : PCI Read Routine                                        */
/* Notes           : Might need a Semaphore                                  */
/*                                                                           */
/*---------------------------------------------------------------------------*/
void     pci_read (usys reg, usys *val)
{
    out32 (0xcf8, reg)  ;
    *val = in32 (0xcfc) ;
} /* End of function pci_read() */



/*---------------------------------------------------------------------------*/
/*                                                                           */
/* Function Name   : dev_init                                                */
/* Description     : Initialize all the needed Devices                       */
/* Notes           : Called from bsp_dev_init(). Kernel is initialized, but  */
/*                   multitasking is not enabled nor are interrupts.         */
/*                                                                           */
/*---------------------------------------------------------------------------*/
usys     dev_init (void)
{
    usys status = GOOD ; 
    pci_t pci   ;

    /* Initialize the Clock */
    if (clk_init() != GOOD)
       status = BAD ;

    /* Initialize the Serial Port */
    if (serial_init() != GOOD)
       status = BAD ;

    /* Initialize the Keyboard */
    if (pc_kbd_init () != GOOD)
       status = BAD ;

    pci.pci_write = pci_write ;
    pci.pci_read  = pci_read  ;

    if (pci_init(&pci) != GOOD)
       status = BAD ;

    return status ;

} /* End of function dev_init() */



