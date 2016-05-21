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
/* File Name       : devices.c                                               */
/* Description     : Initialize all the relavent devices                     */
/* Notes           :                                                         */
/*                                                                           */
/*---------------------------------------------------------------------------*/
#include <akalon.h>
#include <stdio.h>

#include "sam9_l9260.h"


extern   void irq_isr (void) ;



/*---------------------------------------------------------------------------*/
/*                                                                           */
/* Function Name   : clock_isr                                               */
/* Description     :                                                         */
/* Notes           : Called by the interrupt task                            */
/*                                                                           */
/*---------------------------------------------------------------------------*/
void     clock_isr (void)
{
    volatile u32 *reg_addr, val ;

    /* Read the PIT_SR register to see if the interrupt went off */
    reg_addr = (u32 *) 0xfffffd34 ;

    if ((*reg_addr & 0x1) == 0)
       return ;

    /* Now read the PIT_PIVR register to clear the interrupt */
    reg_addr = (u32 *) 0xfffffd38 ;
    val = *reg_addr ;

    /* Tell the OS of the tic */
    timer_tic() ;

} /* End of Function clock_isr() */



/*---------------------------------------------------------------------------*/
/*                                                                           */
/* Function Name   : pit_init                                                */
/* Description     : Initialize all Periodic Interval Timer (PIT)            */
/* Notes           :                                                         */
/*                                                                           */
/*---------------------------------------------------------------------------*/
void     pit_init (void)
{
    volatile u32  *reg_addr, val ;

    /* Set the PIT_MR (PIT Mode Register) */
    val  = 0x000fffff ;  /* PIV (Periodic Interval Value) */
    val |= 0x03000000 ;  /* Enable the Timer (PITEN) and  */
                         /* the interrupt (PITIEN)        */

    reg_addr  = (u32 *) 0xfffffd30 ;
    *reg_addr = val ;

} /* End of function pit_init() */

usys x = 0 ;
/*---------------------------------------------------------------------------*/
/*                                                                           */
/* Function Name   : aic_isr                                                 */
/* Description     :                                                         */
/* Notes           : Called by the interrupt task                            */
/*                                                                           */
/*---------------------------------------------------------------------------*/
void     aic_isr (void)
{
    volatile u32 *reg_addr, val ;

    /* Read the Interrupt Pending Register (AIC_IPR) */
    reg_addr = (volatile u32 *) 0xfffff10c ;
    val = *reg_addr ;

    /* Check units connected to int 1 */
    if (val & 0x2)
    {
       clock_isr()  ;
       uart_isr()   ;
    } 

    if (val & 0x00200000)
       emac_isr() ;

    /* Clear the Interrupt */
    reg_addr = (volatile u32 *) 0xfffff128 ;
    *reg_addr = val ;

    /* Notify the end of interrupts...*/
    reg_addr  = (u32 *) 0xfffff130 ;
    *reg_addr = 0 ;

    x ++ ;
} /* End of function aic_isr() */



/*---------------------------------------------------------------------------*/
/*                                                                           */
/* Function Name   : aic_init                                                */
/* Description     : Initialize all Advance Interrupt Controller (AIC)       */
/* Notes           :                                                         */
/*                                                                           */
/*---------------------------------------------------------------------------*/
void     aic_init (void)
{
    volatile u32  *reg_addr, val ;

    /* Connect and Enable the ISR */
    int_config (0, ENABLE, aic_isr) ;

    /* Program the Source Vector Register for PID1 (clock and uart) */
    reg_addr  = (volatile u32 *) 0xfffff084 ;
    *reg_addr = (u32) irq_isr ;

    /* Program the Source Vector Register for PID21 (ethernet emac) */
    reg_addr  = (volatile u32 *) 0xfffff0d4 ;
    *reg_addr = (u32) irq_isr ;

    /* Set/Enable the PIT Interrupt, which is or'd to the SYS */
    /* interrupt. The SYS interrupt is mapped to the Source 1 */
    /* interrupt on the AIC. Also, the SYS interrupt goes to  */
    /* the processor as a regular interrupt.                  */

    /* Set the Interrupt Source Mode Registers */

    /* Set the AIC_SMR1 */
    reg_addr  = (volatile u32 *) 0xfffff004 ;
    val       = 0x07 ;  /* SRCTYPE = None for internal devices  */
                        /* PRIOR   = 7 (Highest Priority)       */
    *reg_addr = val ;

    /* Set the AIC_SMR21 */
    reg_addr  = (volatile u32 *) 0xfffff054 ;
    val       = 0x07 ;  /* SRCTYPE = None for internal devices  */
                        /* PRIOR   = 7 (Higest Priority)        */
    *reg_addr = val ;

    /* Enable Interrupts by writing to the Enable Cmd Register (IECR) */
    /* The interrupts that are enabled are the SYS (Clock, UART) and  */
    /* the Ethernet Interurupt (PID21)                                */
    reg_addr  = (volatile u32 *) 0xfffff120 ;
    *reg_addr = 0x00200002 ;

} /* End of function aic_init() */




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
    /* Setup/Enable the PIT (i.e. clock) */
    pit_init() ;

    /* Setup/Enable the AIC (Interrupt Controller) */
    aic_init() ;

    /* Setup/Enable the Serial Port (i.e. debug serial port) */
    uart_init() ;

    /* Setup/Enable the EMAC Network device */
    emac_init() ;

    return GOOD ; 
} /* End of function dev_init() */

