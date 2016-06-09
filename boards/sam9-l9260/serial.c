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
/* Description     : Serial Port on the Olimex SAM9-l9260                    */
/* Notes           : The Serial Port is located in the Debug Unit            */
/*                                                                           */
/*---------------------------------------------------------------------------*/
#include <akalon.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

/* Configuration Start */

#define  NUM_DEVS  2
#define  BUF_SIZE  10

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

    u8   tx_buf     [BUF_SIZE] ;
    usys tx_p       ; /* Producer */
    usys tx_c       ; /* Consumer */
    usys tx_t       ; /* Temp - For Consumer Catchups */
    usys tx_sem     ;

} devT ;

static   devT   *dev = NULL ;
static   usys   rx_task_id  ;
static   usys   tx_task_id  ;

link_t   uart_link ;



/*---------------------------------------------------------------------------*/
/*                                                                           */
/* Function Name   : rx_task                                                 */
/* Description     : Data from This Module -> Top Module                     */
/* Notes           :                                                         */
/*                                                                           */
/*---------------------------------------------------------------------------*/
static   void      rx_task (void)
{
    volatile u32 *reg ;

    while (1) 
    {
       task_sleep() ;

       /* Woken up by the ISR */

       /* Start sending charcters to the module above */
       while (dev->rx_c != dev->rx_p)
       {
	  /* Send the Data */
	  if (uart_link.tx_func != NULL)
             uart_link.tx_func (0, 1, &dev->rx_buf [dev->rx_c % BUF_SIZE]) ;

          /* If the Rx is shut down, then restart it */
          if ((dev->rx_p % BUF_SIZE) == (dev->rx_c++ % BUF_SIZE))
          {
             reg  = (volatile u32 *) 0xFFFFF208 ;
             *reg = 0x1 ;
          }       
       }
    } /* while (1) */

} /* End of function rx_task() */



/*---------------------------------------------------------------------------*/
/*                                                                           */
/* Function Name   : rx_func                                                 */
/* Description     : Data from Top Module -> This Module                     */
/* Notes           :                                                         */
/*                                                                           */
/*---------------------------------------------------------------------------*/
static   usys      rx_func (usys arg, usys size, void *buf)
{
    volatile u32 *reg ;

    if (sem_get (dev->tx_sem, WAIT_FOREVER) != GOOD)
       return BAD ;

    /* Store the data */
    dev->tx_buf [dev->tx_p++ % BUF_SIZE] = *(u8 *) buf ;

    /* If the Transmitter is off, then switch it back on */
    reg  = (volatile u32 *) 0xFFFFF210 ;
    if ((*reg & 0x2) == 0) 
    {
       reg  = (volatile u32 *) 0xFFFFF208 ;
       *reg = 0x2 ;
    } 

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
    task_sleep() ;

    while (1) 
    {
       /* Release all the slots that were sent out */
       while (dev->tx_t <= dev->tx_c)
       {  
          sem_give (dev->tx_sem) ;
	  dev->tx_t ++ ;
       }

       /* Wait for the ISR to wake us up */
       task_sleep() ;
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
    u32 mask_val, int_val ;

    /* Read the Mask Register */
    reg = (volatile u32 *) 0xFFFFF210 ;
    mask_val = *reg ;

    /* Read the Status Register */
    reg = (volatile u32 *) 0xFFFFF214 ;
    int_val = *reg ;

    /* Check the Tx interrupt */
    if ((mask_val & 0x2) && (int_val & 0x2))
    {
       /* Write one character */
       if (dev->tx_c != dev->tx_p)
       {
          /* Write Data to TX Register */
          reg  = (volatile u32 *) 0xFFFFF21c ;
          *reg = dev->tx_buf [dev->tx_c++ % BUF_SIZE] ;

          /* Wake-up the Tx Task. It needs to release tx_sem */ 
          task_wake (tx_task_id) ;
       } else {
          /* Shut down the Transmitter */
          reg  = (volatile u32 *) 0xFFFFF20c ;
          *reg = 0x2 ; 
       }
    }

    /* Check the Rx */
    if ((mask_val & 0x1) && (int_val & 0x1))
    {
       /* Wake up the rx_task */
       task_wake (rx_task_id) ;

       reg = (volatile u32 *) 0xFFFFF218 ;

       /* Copy the Data */
       dev->rx_buf [dev->rx_p++ % BUF_SIZE] = *reg & 0xff ;

       /* If the rx_p is equal to the rx_c, then shut down the receiver */
       if ((dev->rx_p % BUF_SIZE) == (dev->rx_c % BUF_SIZE))
       {
          reg  = (volatile u32 *) 0xFFFFF20c ;
          *reg = 0x1 ;
       }
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

    /* Semaphore */
    if (sem_new (BUF_SIZE, BUF_SIZE, "uart: tx_sem", &dev->tx_sem) != GOOD)
       goto err ;

    /* Indexes */
    dev->tx_p = dev->tx_c = dev->tx_t = 0 ;

    /*** Receiver (Rx) ***/

    /* Task */
    stat = task_new (1, 0, 1024, 0,0, rx_task, 0,0,0, "uart_rx_task", 
                     &rx_task_id) ;
    if (stat != GOOD)
    {
       printf ("ERR: Creating uart_rx_task. Stat = %d\n", stat) ;
       goto err ;
    }

    /* Indexes */
    dev->rx_p = dev->rx_c = 0 ;

    /* Reset the Status Bits */
    reg = (volatile u32 *) (0xFFFFF200) ;
    *reg = 0x100 ;

    /* Enable the Rx Interrupt */
    reg = (volatile u32 *) (0xFFFFF208) ;
    *reg = 0x1 ;

    return GOOD ;

 err:
    /* <-- DO */
    return BAD ;

} /* End of function uart_init() */


