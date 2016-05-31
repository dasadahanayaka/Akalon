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
/* File Name       : serial.c                                                */
/* Description     : Serial Port on the Raspberry PI (PL011 - ARM)           */
/* Notes           : This is the shittiest UART I have ever come across.     */
/*                   Who on earth would design a Rx FIFO which doesn't       */
/*                   interrupt when just 1 byte has been received !$(*&@!    */
/*                   Nhodhokin !!                                            */
/*---------------------------------------------------------------------------*/
#include <akalon.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

/* Registers */

#define  REG_DR    0x20201000
#define  REG_FR    0x20201018
#define  REG_LCRH  0x2020102c
#define  REG_CR    0x20201030
#define  REG_IMSC  0x20201038
#define  REG_MIS   0x20201040
#define  REG_ICR   0x20201044


/* Configuration Start */

#define  NUM_DEVS  1
#define  BUF_SIZE  64

/* Configuration End   */


typedef  struct    devT
{
    void (*reg_write) (usys reg, usys val) ;
    usys (*reg_read)  (usys reg) ;
    usys base_addr  ;

    usys clk_freq   ; /* In Hz */
    usys baud_rate  ;
    usys parity     ;
    usys stop_bits  ;
    usys data_bits  ;

    u8   rx_buf     [BUF_SIZE] ;
    usys rx_p       ; /* Producer */
    usys rx_c       ; /* Consumer */
    usys rx_sem     ;

    usys tx_p       ; /* Producer */
    usys tx_c       ; /* Consumer */
    usys tx_sem     ;
} devT ;

static   devT   *dev = NULL ;
static   usys   rx_task_id  ;
static   usys   tx_task_id  ;

link_t   uart_link ;


/*---------------------------------------------------------------------------*/
/*                                                                           */
/* Function Name   : rx_task                                                 */
/* Description     : Rx Task                                                 */
/* Notes           :                                                         */
/*                                                                           */
/*---------------------------------------------------------------------------*/
static   void      rx_task (void)
{
    volatile u32 *reg ;
    u32 val ;

    /* Enable the Rx Interrupt by setting the mask */
    reg  = (volatile u32 *) REG_IMSC ;
    val  = *reg ;
    *reg = val | 0x10 ;

    while (1) 
    {
       /* Go to sleep. The ISR will wake us up */       
       task_sleep() ;

       while (dev->rx_c != dev->rx_p)
       {
          /* Send the Data */
	  if (uart_link.tx_func != NULL)
	     uart_link.tx_func (0, 1, &dev->rx_buf[dev->rx_c++ % BUF_SIZE]) ; 
       }
    }

} /* End of function rx_task() */



/*---------------------------------------------------------------------------*/
/*                                                                           */
/* Function Name   : rx_func                                                 */
/* Description     : Handles input from the module above                     */
/* Notes           :                                                         */
/*                                                                           */
/*---------------------------------------------------------------------------*/
static   usys      rx_func (usys type, usys bufSize, void *buf)
{
    volatile u32 *reg ;
    u32 val ;

    /* Send Data */
    reg  = (volatile u32 *) REG_DR ;
    *reg = *(u8 *) buf ;

    /* Enable TX int */
    reg  = (volatile u32 *) REG_IMSC ;
    val  = *reg ;
    *reg = val | 0x20 ;

    /* Wait for Sem */
    if (sem_get (dev->tx_sem, WAIT_FOREVER) != GOOD)
       return BAD ;

    return GOOD ;

} /* End of function rx_func() */



/*---------------------------------------------------------------------------*/
/*                                                                           */
/* Function Name   : tx_task                                                 */
/* Description     : Tx Task                                                 */
/* Notes           :                                                         */
/*                                                                           */
/*---------------------------------------------------------------------------*/
static   void      tx_task (void)
{
    while (1) 
    {
       /* Go to sleep. The ISR will wake... */
       task_sleep() ;

       /* Give the Tx Semaphore */
       sem_give (dev->tx_sem) ;
    }

} /* End of function tx_task() */



/*---------------------------------------------------------------------------*/
/*                                                                           */
/* Function Name   : uart_isr                                                */
/* Description     :                                                         */
/* Notes           :                                                         */
/*                                                                           */
/*---------------------------------------------------------------------------*/
void     uart_isr (void)
{
    volatile u32 *reg ;
    u32 mis_val, val  ;

    /* Read the Masked Interrupt Status Register */
    mis_val = * ((volatile u32 *) REG_MIS) ;

    /* And clear the Interrupt by writing to the ICR */
    reg  = (volatile u32 *) REG_ICR ;
    val  = *reg ;
    *reg = val  ;

    /* Check the Tx interrupt */
    if (mis_val & 0x20)
    {
       /* Disable the TX Interrupt */
       reg  = (volatile u32 *) REG_IMSC ;
       val  = *reg ;
       *reg = val & 0x7d2 ; 

       /* Wake up the rx_task */
       task_wake (tx_task_id) ;
    } 

    /* Check the Rx interrupt */
    if (mis_val & 0x10)
    {
       /* Read all the received data */
       while (1)
       {
	  /* Check if there is no more data */
          reg = (volatile u32 *) REG_FR ;
          if (*reg & 0x10)
	     break ;

          /* Read the Data Register */
          reg = (volatile u32 *) REG_DR ;
          val = *reg ;

          /* Check for Errors */
          if (val & 0x700)
	     continue ;

          /* Copy the received data */
          dev->rx_buf [dev->rx_p++ % BUF_SIZE] = val & 0xff ;
       } 

       /* Wake up the rx_task */
       task_wake (rx_task_id) ;
    }

} /* End of function uart_isr() */



/*---------------------------------------------------------------------------*/
/*                                                                           */
/* Function Name   : uart_init                                               */
/* Description     : Initialization routine                                  */
/* Notes           : Called after os_init()                                  */
/*                                                                           */
/*---------------------------------------------------------------------------*/
usys     uart_init (void)
{
    volatile u32 *reg ;
    usys stat ;

    /* Malloc dev struct */

    if ((dev = malloc (sizeof (devT))) == NULL)
       return E_MEM ;

    /* Now Initialize dev struct */     
    memset (dev, 0, sizeof (devT)) ;

    /* Initialize the Interface */
    memset (&uart_link, 0, sizeof (link_t)) ;
    uart_link.rx_func = rx_func ;

    /*** Transmitter (Tx) ***/

    /* Task */
    stat = task_new (1, 0, 1024, 0,0, tx_task, 0,0,0, "uart_tx_task", 
                     &tx_task_id) ;
    if (stat != GOOD)
    {
       printf ("ERR: Creating uart_tx_task. Stat = %d\n", stat) ;
       goto err ;
    }

    /* Semaphores */
    if (sem_new (1, 1, "uart: tx_sem", &dev->tx_sem) != GOOD)
       goto err ;

    /* Indexes */
    dev->tx_p = dev->tx_c = 0 ;

    /*** Receiver (Rx) ***/

    /* Task */
    stat = task_new (1, 0, 1024, 0,0, rx_task, 0,0,0, "uart_rx_task", 
                     &rx_task_id) ;
    if (stat != GOOD)
    {
       printf ("ERR: Creating uart_rx_task. Stat = %d\n", stat) ;
       goto err ;
    }

    /* Semaphore */
    if (sem_new (BUF_SIZE, 0, "uart: rx_sem", &dev->rx_sem) != GOOD)
       goto err ;

    /* Indexes */
    dev->rx_p = dev->rx_c = 0 ;

    /************************/
    /* Device Configuration */
    /************************/

    /* Disable the UART */
    reg = (volatile u32 *) REG_CR ;
    *reg = 0 ;

    /* No need to wait for the Tx or the Rx of the data, since */
    /* this is the init condition of the driver.               */

    /* Flush the FIFOs */
    reg = (volatile u32 *) REG_LCRH ;
    *reg = 0 ;

    /* ---------------------------*/
    /* Set the Configuration to.. */
    /*                            */
    /* Send Break   = No          */
    /* Parity       = Disabled    */
    /* Data Bits    = 8           */
    /* Parity Type  = Odd         */
    /* Stop Bits    = 1           */
    /* FIFOs        = Disabled    */
    /* Stick Parity = Disabled    */
    /* ---------------------------*/

    *reg = 0x60 ; 

    /* Reset the Status Bits */
    reg = (volatile u32 *) REG_ICR ;
    *reg = 0xffffffff ; 

    /* Enable Tx and Rx */
    reg = (volatile u32 *) REG_CR ;
    *reg = 0x300 ; /* Enable Tx and Rx */
    *reg = 0x301 ; /* Enable the UART  */

    return GOOD ;

 err:
    /* <-- DO */
    return BAD ;

} /* End of function uart_init() */


