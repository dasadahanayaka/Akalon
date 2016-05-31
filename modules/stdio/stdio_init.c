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
/* File Name       : stdio_init.c                                            */
/* Description     : Standard Input/Output Module                            */
/* Notes           :                                                         */
/*                                                                           */
/*---------------------------------------------------------------------------*/
#include <akalon.h>
#include <string.h>
#include <stdio.h>

#define  BUF_SIZE  1024

link_t   stdio_link  ;

static   u8    rx_buf [BUF_SIZE] ;
static   usys  rx_sem ;
static   usys  rx_c   ; /* Consumer */
static   usys  rx_p   ; /* Producer */
 
static   usys  out_inited = NO ;
static   u8    out_buf [BUF_SIZE] ;
static   usys  out_free_sem ;
static   usys  out_buf_sem  ;
static   usys  out_task_sem ;

static   usys  out_c ;
static   usys  out_p ;




/*---------------------------------------------------------------------------*/
/*                                                                           */
/* Function Name   : getchar                                                 */
/* Description     : Sends back a charcter from stdin                        */
/* Notes           : This call can Block                                     */
/*                                                                           */
/*---------------------------------------------------------------------------*/
int      getchar (void)
{
    int   val ;

    /* What if multiple tasks call getchar <-- DO */
    if (sem_get (rx_sem, WAIT_FOREVER) == GOOD)
    {
       val = rx_buf [rx_c++ % BUF_SIZE] ;
       return val ;
    }

    return EOF ;

} /* End of Function getchar () */



/*---------------------------------------------------------------------------*/
/*                                                                           */
/* Function Name   : rx_func                                                 */
/* Description     : stdio's receive function                                */
/* Notes           :                                                         */
/*                                                                           */
/*---------------------------------------------------------------------------*/
static   usys      rx_func (usys arg, usys size, void *data)
{
    if (sem_give (rx_sem) == GOOD)
    {
       rx_buf [rx_p++ % BUF_SIZE] = *(u8 *) data ;
       return GOOD ;
    }

    return BAD ;
} /* End of function rx_func() */



/*---------------------------------------------------------------------------*/
/*                                                                           */
/* Function Name   : putchar                                                 */
/* Description     : Puts a charcter into stdout                             */
/* Notes           : This call can Block                                     */
/*                                                                           */
/*---------------------------------------------------------------------------*/
int      putchar (int c)
{
    int bl = '\r' ;

    if (out_inited == YES)
    {
       if (sem_get (out_buf_sem, WAIT_FOREVER) != GOOD)
          return EOF ;

       /* First take the buffer semaphore */
       if (sem_get (out_free_sem, WAIT_FOREVER) != GOOD)
          return EOF ;

       /* Copy character into buffer */
       out_buf [out_p++ % BUF_SIZE] = (u8) c ;

       /* Give back the buffer semaphore */
       sem_give (out_free_sem) ;

       /* Now give the rcv sem which will unblock the output task */
       sem_give (out_task_sem) ;

    } else {
       /* The stdout is still not inited. Used the Debug out */
       dbg_putchar (c) ;

       /* Follow a new line char with a begin line char */ 
       if (c == '\n') 
          dbg_putchar (bl) ;
    }

    return c ;

} /* End of function putchar() */



/*---------------------------------------------------------------------------*/
/*                                                                           */
/* Function Name   : stdout_task                                             */
/* Description     : Standard Output Task                                    */
/* Notes           :                                                         */
/*                                                                           */
/*---------------------------------------------------------------------------*/
void      stdout_task (void)
{
    u8    c, bl = '\r' ;

    /* When the stdout_task is first run, make sure to take putchar */
    /* out of the debug state and start using interrupts.           */
    out_inited = YES ;

    for (;;)
    {
       /* Wait for a Slot to be available */
       if (sem_get (out_task_sem, WAIT_FOREVER) != GOOD)
       {
          /* <-- DO */
       }

       c = out_buf [out_c++ % BUF_SIZE] ;

       /* Send data to the receiver module */
       if (stdio_link.tx_func != NULL)
       {
          stdio_link.tx_func (0, 1, &c) ;

	  /* Follow a new line char with a begin line char */ 
          if (c == '\n')
             stdio_link.tx_func (0, 1, &bl) ;
       }

       sem_give (out_buf_sem) ;
    }

} /* End of function stdout_task () */



/*---------------------------------------------------------------------------*/
/*                                                                           */
/* Function Name   : stdio_init                                              */
/* Description     : STDIO Initialization                                    */
/* Notes           :                                                         */
/*                                                                           */
/*---------------------------------------------------------------------------*/
usys     stdio_init (void)
{
    usys stat ;
    usys id ;

    /********************/
    /* Initialize Input */
    /********************/

    /* Create a counting semaphore (Full) */
    /* for the input buffer               */

    stat = sem_new (BUF_SIZE, 0, "STDIO: in_buf_sem", &rx_sem) ;
    if (stat != GOOD)
    {
       printf ("ERR: (STDIO) Creating in_buf_sem. Stat = %d\n", stat) ;
       /* Graceful Exit <-- DO */
       return BAD ;
    }

    rx_p = rx_c = 0 ;

    /*********************/
    /* Initialize Output */
    /*********************/

    /* STD-Output Task */
    stat = task_new (2, 0, 1024, 0,0, stdout_task, 0,0,0, "stdout_task", &id) ;
    if (stat != GOOD)
    {
       printf ("ERR: (STDIO) Creating stdout_task. Stat = %d\n", stat) ;
       /* Graceful Exit <-- DO */
       return BAD ;
    }

    /* Create a binary semaphore (Full) to protect */
    /* the out_free variable                       */

    stat = sem_new (1, 1, "STDIO: out_free_sem", &out_free_sem) ;
    if (stat != GOOD)
    {
       printf ("ERR: (STDIO) Creating out_free_sem. Stat = %d\n", stat) ;
       /* Graceful Exit <-- DO */
       return BAD ;
    }

    /* Create a counting semaphore (Empty) to wake-up */
    /* the Output Task when data is ready             */

    stat = sem_new (BUF_SIZE, 0, "STDIO: out_task_sem",
                    &out_task_sem) ;

    if (stat != GOOD)
    {
       printf ("ERR: Cannot Create STDIO: out_task_sem. Stat = %d\n", stat) ;
       /* Graceful Exit <-- DO */
       return BAD ;
    }

    /* Create a counting semaphore (Full) that */
    /* gives a slot in the output buffer       */

    stat = sem_new (BUF_SIZE, BUF_SIZE, "STDIO: out_buf_sem", &out_buf_sem) ;
    if (stat != GOOD)
    {
       printf ("ERR: (STDIO) Creating out_buf_sem. Stat = %d\n", stat) ;
       /* Graceful Exit <-- DO */
       return BAD ;
    }

    out_p = out_c = 0 ;

    /* Initialize the stdio interface structure */
    memset (&stdio_link, 0, sizeof (link_t)) ;
    stdio_link.rx_func = rx_func ;

    return GOOD ;

} /* End of function stdio_init() */


