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

#include "raspberry-pi.h"


/*---------------------------------------------------------------------------*/
/*                                                                           */
/* Function Name   : sys_timer_isr                                           */
/* Description     : System Timer ISR                                        */
/* Notes           : Called from an Interrupt Context                        */
/*                                                                           */
/*---------------------------------------------------------------------------*/
void     sys_timer_isr (void)
{
    volatile u32 *reg_addr, val ;

    /* First Clear the System Timer Control/Status Bit Register */
    reg_addr  = (u32 *) 0x20003000 ;
    val       = *reg_addr & 0xf ;
    *reg_addr = val ;

    /* Check if it was the Counter 2 (C1) that went off...*/
    if ((val & 0x2) == 0)
       return ;

    /* Read the System Timer Counter (Lower) Register */
    reg_addr = (u32 *) 0x20003004 ;
    val = *reg_addr ;

    /* Add the next interrupt count. Akalon's standard tic is 1/100 sec,  */
    /* which is 10 mSec. Since the System timer counter is run off a 1MHz */
    /* clock, it counts up every 1 uSec. Thus, to get a standard Akalon   */
    /* tic interrupt, the counter needs to increment 10 * 1000 times.     */

    val += 10000 ;

    /* Now write the System Timer Compare (C1) register */
    reg_addr  = (u32 *) 0x20003010 ;
    *reg_addr = val ;

    /* Tell the OS of the tic */
    timer_tic() ;

} /* End of Function sys_timer_isr() */



/*---------------------------------------------------------------------------*/
/*                                                                           */
/* Function Name   : int_ctrl_isr                                            */
/* Description     : Interrupt Controller ISR                                */
/* Notes           : Called by the interrupt task                            */
/*                                                                           */
/*---------------------------------------------------------------------------*/
void     int_ctrl_isr (void)
{
    volatile u32 *reg_addr, val ;

    /* Read the IRQ1 Pending Register */
    reg_addr = (u32 *) 0x2000b204 ;
    val = *reg_addr ;

    /* Now parse the interrupts */
    if (val & 0x2)
       sys_timer_isr() ; /* Timer */

    reg_addr = (u32 *) 0x2000b208 ;
    val = *reg_addr ;

    if (val & 0x02000000)
       uart_isr() ; /* UART */

} /* End of function int_ctrl_isr() */



/*---------------------------------------------------------------------------*/
/*                                                                           */
/* Function Name   : sys_timer_init                                          */
/* Description     : Initialize the System Timer                             */
/* Notes           : Use System Timer 1 for the Sys Clock                    */
/*                                                                           */
/*---------------------------------------------------------------------------*/
void     sys_timer_init (void)
{
    volatile u32  *reg_addr, val ;

    /* Clear any pending bits in the System Timer Control/Status   */
    /* register (CS).                                              */
    reg_addr   = (u32 *) 0x20003000 ;
    val        = *reg_addr & 0xf ;
    *reg_addr  = val ;

    /* Enable the System Timer. This is done by reading the System */
    /* Timer Counter Lower Register and then adding a constant to  */
    /* that value and writing it to the System Timer Compare C0    */
    /* register.                                                   */

    reg_addr  = (u32 *) 0x20003004 ; /* CLO Register */
    val = *reg_addr ;

    val += 0x100000 ;

    reg_addr  = (u32 *) 0x20003010 ; /* C1 register */
    *reg_addr = val ;    


} /* End of function sys_timer_init_init() */



/*---------------------------------------------------------------------------*/
/*                                                                           */
/* Function Name   : int_ctrl_init                                           */
/* Description     : Initialize the Interrupt Controller                     */
/* Notes           :                                                         */
/*                                                                           */
/*---------------------------------------------------------------------------*/
void     int_ctrl_init (void)
{
    u32  *reg_addr, val ;

    /* Connect and Enable the ISR */
    int_config (0, ENABLE, int_ctrl_isr) ;

    /* Enable the Timer Interrupt. This interrupt */
    /* is located in the IRQ 1 register set.      */

    reg_addr  = (u32 *) 0x2000b210 ; /* IRQ1 Enable Register */
    val       = *reg_addr ; 
    *reg_addr = val | 0x2 ; /* Enable C1 */


    /* Enable the UART Interrupt. This interrupt  */
    /* is located in the IRQ 2 register set.      */

    reg_addr  = (u32 *) 0x2000b214 ; /* IRQ2 Enable Register */
    val       = *reg_addr ; 
    *reg_addr = val | 0x02000000 ; /* Enable  */

} /* End of function int_ctrl_init() */



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
    /* Setup/Enable the System Timer (i.e. the "tic") */
    sys_timer_init() ;

    /* Setup/Enable the Interrupt Controller */
    int_ctrl_init() ;

    /* Setup/Enable the Serial Port */
    uart_init() ;

    return GOOD ; 
} /* End of function dev_init() */


