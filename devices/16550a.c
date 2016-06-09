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
/* File Name       : 16550a.c                                                */
/* Description     : Most widely used Serial Device                          */
/* Notes           : This is a 8250/16450 with FIFOs.                        */
/*                                                                           */
/*---------------------------------------------------------------------------*/
#include <akalon.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

/* Configuration Start */

#define  NUM_DEVS  2
#define  BUF_SIZE  16

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
    usys tx_sem_loc ;
    usys tx_sem     ;
    usys tx_size    ;

} devT ;

static   devT   *dev = NULL ;
static   usys   rx_task_id  ;
static   usys   tx_task_id  ;


link_t   ns16550_link ;

#define  REG_TX    0x0
#define  REG_RX    0x0

#define  REG_DLRL  0x0
#define  REG_DLRU  0x1

#define  REG_IER   0x1
#define  REG_IIR   0x2

#define  REG_LCR   0x3
#define  REG_LSR   0x5

#define  REG_MCR   0x4
#define  REG_MSR   0x6

#define  REG_SCR   0x7



/*---------------------------------------------------------------------------*/
/*                                                                           */
/* Function Name   : baud_rate_set                                           */
/* Description     : Set the Baud Rate                                       */
/* Notes           :                                                         */
/*                                                                           */
/* This is how the divisor register is calculated.                           */
/*                                                                           */
/* 1) Time for each baud = 1 / Baud Rate                                     */
/*                                                                           */
/* 2) Uarts sample each baud 16 times and take the average. Therefore, each  */
/*    Baud needs to be sampled 16 more times than (1).                       */
/*                                                                           */
/*    Time for each baud = 1 / (BaudRate * 16)                               */
/*                                                                           */
/* 3) Each Uart is fed a clock and on each clock, the divisor is incremented */
/*    a notch. Therefore...                                                  */
/*                                                                           */
/*    Each notch on the Divisor is equal to time = 1 / Freq                  */
/*                                                                           */
/*    This gives how much time a notch takes.                                */
/*                                                                           */
/* 4) So to see how many notches we need, take the needed time (2) for the   */
/*    sampling rate and divide it by the time for each notch (3).            */
/*                                                                           */
/*    Notches needed = (1 / (BaudRate * 16)) / (1 / Freq)                    */
/*                                                                           */
/*                   = Freq / (1 / (BaudRate * 16))                          */
/*                                                                           */
/*---------------------------------------------------------------------------*/
static   usys      baud_rate_set (devT *dev, usys baud_rate)
{
    usys val ;
    u8   cur ;

    /* Error Checking */
    if (dev == NULL)
       return BAD ;

    /* No need to set it if it's already set */
    if (dev->baud_rate == baud_rate)
       return GOOD ;

    val = dev->clk_freq / (baud_rate * 16) ;

    /* First set the DLAB Bit. But before doing that, save */
    /* current state of the register.                      */

    cur = dev->reg_read (dev->base_addr + REG_LCR) & 0x7f ;
    dev->reg_write (dev->base_addr + REG_LCR, cur | 0x80) ;

    /* Now write the Divisor Regs */
    dev->reg_write (dev->base_addr + REG_DLRL, val & 0xff) ;
    dev->reg_write (dev->base_addr + REG_DLRU, val >> 8)   ;

    /* Restore the LCR */
    dev->reg_write (dev->base_addr + REG_LCR, cur) ; 

    dev->baud_rate = baud_rate ;

    return GOOD ;

} /* End of Function baud_rate_set () */



/*---------------------------------------------------------------------------*/
/*                                                                           */
/* Function Name   : config_set                                              */
/* Description     : Sets the Parity, Stop bits and the number of data bits  */
/* Notes           :                                                         */
/*                                                                           */
/*---------------------------------------------------------------------------*/
static   usys      config_set (devT *dev, usys parity, usys stop_bits, 
                               usys data_bits)
{
    u8   val = 0 ;

    /* Error Checking */
    if (dev == NULL)
       return BAD ;

    switch (parity)
    {
        case PARITY_NONE:
             break ;

        case PARITY_ODD:
             val = 0x1 << 3 ;
             break ;

        case PARITY_EVEN:
             val = 0x3 << 3 ;
             break ;

        case PARITY_MARK:
             val = 0x5 << 3 ;
             break ;

        case PARITY_SPACE:
             val = 0x7 << 3 ;
             break ;

        default:
             return BAD ;
    }

    switch (stop_bits)
    {
        case 1:
             break ;

        case 2:
             val |= 0x4 ;
             break ;

        default:
             return BAD ;
    }

    switch (data_bits)
    {
        case 5:
             break ;

        case 6:
             val |= 0x1 ;
             break ;

        case 7:
             val |= 0x2 ;
             break ;

        case 8:
             val |= 0x3 ;
             break ;

        default:
             return BAD ;
    }

    /* Now write the value */
    dev->reg_write (dev->base_addr + REG_LCR, val) ; 

    dev->parity    = parity    ;
    dev->stop_bits = stop_bits ;
    dev->data_bits = data_bits ;

    return GOOD ;

} /* End of function setConfig () */



/*---------------------------------------------------------------------------*/
/*                                                                           */
/* Function Name   : rx_task                                                 */
/* Description     : Data from This Module -> Top Module                     */
/* Notes           :                                                         */
/*                                                                           */
/*---------------------------------------------------------------------------*/
static   void      rx_task (void)
{
    /* Enable the Receiver, ERR + Break */
    dev->reg_write (dev->base_addr + REG_IER, 0x5) ;

    while (1) 
    {
       task_sleep() ;

       /* Get caught up to the producer */
       while (dev->rx_c != dev->rx_p)
       {
	  if (ns16550_link.tx_func != NULL)
             ns16550_link.tx_func (0,1, &dev->rx_buf [dev->rx_c % BUF_SIZE]) ;

          /* If the Rx is shut down, then restart it */
          if ((dev->rx_p % BUF_SIZE) == (dev->rx_c++ % BUF_SIZE))       
              dev->reg_write (dev->base_addr + REG_IER, 
                              dev->reg_read (dev->base_addr + REG_IER) | 0x1) ;

       }
    }

} /* End of function rx_task() */



/*---------------------------------------------------------------------------*/
/*                                                                           */
/* Function Name   : rx_func                                                 */
/* Description     : Data from Top Module -> This Module                     */
/* Notes           :                                                         */
/*                                                                           */
/*---------------------------------------------------------------------------*/
static   usys      rx_func (usys type, usys bufSize, void *buf)
{
    if (sem_get (dev->tx_sem, WAIT_FOREVER) != GOOD)
       return BAD ;

    /* Store the data */
    dev->tx_buf [dev->tx_p++ % BUF_SIZE] = *(u8 *) buf ;

    /* If the Transmitter is off, then switch it back on */
    if ((dev->reg_read (dev->base_addr + REG_IER) & 0x2) == 0) 
       dev->reg_write (dev->base_addr + REG_IER, 
                       dev->reg_read (dev->base_addr + REG_IER) | 0x2) ;

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
/* Function Name   : ns16550_isr                                             */
/* Description     :                                                         */
/* Notes           :                                                         */
/*                                                                           */
/*---------------------------------------------------------------------------*/
void     ns16550_isr (void)
{
    usys write_size, i ;
    u8   reg_val ;

    /* Error Checking */
    if (dev == NULL)
       return ;

    while (1)
    {   
       /* Read the IIR for an interrupt */ 
       if ((reg_val = dev->reg_read (dev->base_addr + REG_IIR)) & 0x1)
	  break ;

       /* Check Errors -- Priority 1 */
       if ((reg_val & 0x6) == 0x6)
       {
          dev->reg_read (dev->base_addr + REG_LSR) ;
	  continue ;
       }

       /* Check Receiver -- Priority 2 */
       if ((reg_val & 0x6) == 0x4) 
       {
          /* Wake up the rx_task */
          task_wake (rx_task_id) ;

          do 
          {
             /* Copy the Data */
             dev->rx_buf [dev->rx_p++ % BUF_SIZE] = 
                   dev->reg_read (dev->base_addr + REG_RX) ; 

             /* If the rx_p is equal to the rx_c, then */
             /* the receiver needs to be shut down.    */

             if ((dev->rx_p % BUF_SIZE) == (dev->rx_c % BUF_SIZE))
	     { 
                dev->reg_write (dev->base_addr + REG_IER, 
                         dev->reg_read (dev->base_addr + REG_IER) & 0xe) ;
		break ;
	     }

	  } while (dev->reg_read (dev->base_addr + REG_LSR) & 0x1) ;

	  continue ;
       }

       /* Check Transmiter - Priority 3 */
       if ((reg_val & 0x6) == 0x2)
       {
          /*** Tx is Ready. ***/

          /* Wake-up the Tx Task. It needs to release tx_sem */ 
          task_wake (tx_task_id) ;          

          /* Write data until the Producer Index */
	  if (dev->tx_c != dev->tx_p)
	  {
             /* Figure out the maximum data that can be written */
             if (dev->reg_read (dev->base_addr + REG_LSR) & 0x20)
             {
	        write_size = dev->tx_size ;
             } else {
	        write_size = 1 ;
             } 

             for (i = 0; i < write_size; i++)
	     {
	        /* Write Data to TX Register */
                dev->reg_write (dev->base_addr + REG_TX, 
                                dev->tx_buf [dev->tx_c++ % BUF_SIZE]) ;

                /* If there is no more data, then leave this loop. */
                if (dev->tx_c == dev->tx_p) 
                   break ;
	     }

	  } else {

	     /* Shut down the Transmitter */
             dev->reg_write (dev->base_addr + REG_IER, 
                             dev->reg_read (dev->base_addr + REG_IER) & 0xd) ;
	  }
       } /* Tx */
    } /* while(1) */
} /* End of function ns16550_isr() */



/*---------------------------------------------------------------------------*/
/*                                                                           */
/* Function Name   : ns16550_init                                            */
/* Description     : Initialization routine                                  */
/* Notes           :                                                         */
/*                                                                           */
/*---------------------------------------------------------------------------*/
usys     ns16550_init (ns16550_t *ptr)
{
    usys stat ;

    /* Malloc dev struct */

    if ((dev = malloc (sizeof (devT))) == NULL)
       return E_MEM ;

    /* Now Initialize dev struct */     

    memset (dev, 0, sizeof (devT)) ;

    dev->reg_write = ptr->reg_write ;
    dev->reg_read  = ptr->reg_read  ;
    dev->base_addr = ptr->base_addr ;        

    dev->clk_freq  = ptr->clk_freq  ;

    /* Initialize the Device */
    if (config_set (dev, ptr->parity, ptr->stop_bits, ptr->data_bits) != GOOD)
       goto err ;

    if (baud_rate_set (dev, ptr->baud_rate) != GOOD)
       goto err ;

    /* Initialize the Interface */
    memset (&ns16550_link, 0, sizeof (link_t)) ;
    ns16550_link.rx_func = rx_func ;


    /*** Transmitter (Tx) ***/

    /* Task */
    stat = task_new (1, 0, 1024, 0,0, tx_task, 0,0,0, "16550_tx_task", 
                     &tx_task_id) ;
    if (stat != GOOD)
    {
       printf ("ERR: Creating 16550_tx_task. Stat = %d\n", stat) ;
       goto err ;
    }

    /* Semaphore */
    if (sem_new (BUF_SIZE, BUF_SIZE, "16550: tx_sem", &dev->tx_sem) != GOOD)
       goto err ;

    /* Indexes */
    dev->tx_p = dev->tx_c = dev->tx_sem_loc = 0 ;

    /* Test if the UART has a FIFO */
    dev->reg_write (dev->base_addr + REG_IIR, 0xc7) ;
    if ((dev->reg_read (dev->base_addr + REG_IIR) & 0xc0) == 0xc0)
    {  
       dev->tx_size = 16 ;

       /* Set the transmitter to interrupt after 1 character */
       dev->reg_write (dev->base_addr + REG_IIR, 0x07) ;

    } else {
       dev->tx_size = 1  ;
    }

    /*** Receiver (Rx) ***/

    /* Task */
    stat = task_new (1, 0, 1024, 0,0, rx_task, 0,0,0, "16550_rx_task", 
                     &rx_task_id) ;
    if (stat != GOOD)
    {
       printf ("ERR: Creating 16550_rx_task. Stat = %d\n", stat) ;
       goto err ;
    }

    /* Indexes */
    dev->rx_p = dev->rx_c = 0 ;

    /* For the PC, the !OUT2 pin needs to be enabled because it's gated */
    /* with the interrupt pin. Otherwise no interrupts are generated.   */

    dev->reg_write (dev->base_addr + REG_MCR, 0x0b) ;

    return GOOD ;

 err:
    /* <-- DO */
    return BAD ;

} /* End of function ns16550_init() */


