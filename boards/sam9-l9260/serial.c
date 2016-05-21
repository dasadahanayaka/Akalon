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
#define  BUF_SIZE  4096  /* WARNING: The hang bug here <-- DO */

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
    usys rx_sem_loc ;
    usys rx_sem     ;

    u8   tx_buf     [BUF_SIZE] ;
    usys tx_p       ; /* Producer */
    usys tx_c       ; /* Consumer */
    usys tx_sem_loc ;
    usys tx_sem     ;
    usys tx_size    ;

} devT ;

static   devT   *dev = NULL ;
static   usys   rx_task_id  ;
static   usys   tx_task_id  ;


link_t   uart_link ;


/*---------------------------------------------------------------------------*/
/*                                                                           */
/* Function Name   : ui_tx                                                   */
/* Description     : Output from the Serial Driver to above                  */
/* Notes           : This is a Blocking Call                                 */
/*                                                                           */
/*---------------------------------------------------------------------------*/
static   usys      ui_tx (usys type, usys bufSize, u8 *buf, usys *retSize)
{
    volatile u32 *reg ;

    /* See if there is data. Otherwise, wait for it... */
    if (sem_get (dev->rx_sem, WAIT_FOREVER) == GOOD)
    {
       *retSize = 1 ;
       *buf = dev->rx_buf [dev->rx_c % BUF_SIZE] ;

       /* If the Rx is shut down, then restart it */
       if ((dev->rx_p % BUF_SIZE) == (dev->rx_c++ % BUF_SIZE))
       {
          reg  = (volatile u32 *) 0xFFFFF208 ;
          *reg = 0x1 ;
       }       

       return GOOD ;
    }

    return BAD ;
} /* End of function ui_tx() */



/*---------------------------------------------------------------------------*/
/*                                                                           */
/* Function Name   : rx_task                                                 */
/* Description     : Rx Task                                                 */
/* Notes           :                                                         */
/*                                                                           */
/*---------------------------------------------------------------------------*/
static   void      rx_task (void)
{
    while (1) 
    {
       task_sleep() ;

       /* Received character/s. Get caught up with the producer index */
       while (dev->rx_sem_loc != dev->rx_p)
       {
          sem_give (dev->rx_sem) ;
          dev->rx_sem_loc++ ;
       }
    }

} /* End of function rx_task() */



/*---------------------------------------------------------------------------*/
/*                                                                           */
/* Function Name   : ui_rx                                                   */
/* Description     : Input into the Serial Driver from above                 */
/* Notes           :                                                         */
/*                                                                           */
/*---------------------------------------------------------------------------*/
static   usys      ui_rx (usys type, usys bufSize, u8 *buf)
{
    volatile u32 *reg ;

    if (sem_get (dev->tx_sem, WAIT_FOREVER) != GOOD)
       return BAD ;

    /* Store the data */
    dev->tx_buf [dev->tx_p++ % BUF_SIZE] = *buf ;

    /* If this is a new line, send a line feed char too...Damn Silly !!*/
    if (*buf == '\n')
    {
       if (sem_get (dev->tx_sem, WAIT_FOREVER) != GOOD)
          return BAD ;

       dev->tx_buf [dev->tx_p++ % BUF_SIZE] = '\r' ;
    }

    /* If the Transmitter is off, then switch it back on */
    reg  = (volatile u32 *) 0xFFFFF210 ;
    if ((*reg & 0x2) == 0) 
    {
       reg  = (volatile u32 *) 0xFFFFF208 ;
       *reg = 0x2 ;
    } 

    return GOOD ;

} /* End of function ui_rx() */



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
       task_sleep() ;

       /* Transmitted character/s. Get caught up with the producer index */
       while (dev->tx_sem_loc != dev->tx_c)
       {
          sem_give (dev->tx_sem) ;
          dev->tx_sem_loc ++ ;
       }
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
    u32 val ;

    /* Read the Status Register */
    reg = (volatile u32 *) 0xFFFFF214 ;
    val = *reg ;

    /* Check the Tx */
    if (val & 0x2)
    {
       /* Write data until the Producer Index */
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
    if (val & 0x1)
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
    uart_link.ui_rx = ui_rx ;
    uart_link.ui_tx = ui_tx ;

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
    dev->tx_p = dev->tx_c = dev->tx_sem_loc = 0 ;

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
    dev->rx_p = dev->rx_c = dev->rx_sem_loc = 0 ;

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


