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
/* File Name       : msgs.c                                                  */
/* Description     : Akalon Message functions                                */
/* Notes           :                                                         */
/*                                                                           */
/*---------------------------------------------------------------------------*/
#include <akalon.h>
#include <string.h>
#include <stdlib.h>

#include "kernel.h"

/*----------------------*/
/* Message Box Controls */
/*----------------------*/

typedef  struct    msg_t
{
    usys      id        ;
    usys      size      ;
    void      *loc      ;
} msg_t ;


typedef  struct    msg_box_t
{
    usys      max_msgs  ;
    usys      max_size  ;

    usys      num_msgs  ;
    usys      p_index   ; /* Producer */
    usys      c_index   ; /* Consumer */

    msg_t     *msg_buf  ;
} msg_box_t ;



/*---------------------------------------------------------------------------*/
/*                                                                           */
/* Function Name   : msgbox_init                                             */
/* Description     : Allocate and initialize a message box                   */
/* Notes           :                                                         */
/*                                                                           */
/*---------------------------------------------------------------------------*/
usys     msgbox_init (usys max_msgs, usys max_size, void **msg_box_loc)
{
    msg_box_t *msg_box = NULL ;
    msg_t     *msg_buf = NULL ;

    /* Allocate the mbox_t structure and initialize */
    msg_box = malloc (sizeof(msg_box_t)) ;
    if (msg_box == NULL)
       goto err ;

    msg_box->max_msgs = max_msgs ;
    msg_box->max_size = max_size ;

    msg_box->p_index  = 0 ;
    msg_box->c_index  = 0 ;
    msg_box->num_msgs = 0 ;


    /* Send back the location of the mbox_t instance */
    *msg_box_loc = msg_box ;

    /* Allocate the msg buffer */
    if (max_msgs != 0)
    {
       msg_buf = malloc ((max_msgs * sizeof (msg_t)) + 
                         (max_msgs * max_size)) ; 
       if (msg_buf == NULL)
	  goto err ;

    } 

    /* Connect the msg buffer to the mbox */
    msg_box->msg_buf = msg_buf ;

    return GOOD ;

err:
    if (msg_box != NULL)
       free (msg_box) ;

    if (msg_buf != NULL)
       free (msg_buf) ;

    msg_box->msg_buf = NULL ;

    return E_MEM ;

} /* End of function msgbox_init () */



/*---------------------------------------------------------------------------*/
/*                                                                           */
/* Function Name   : msg_get                                                 */
/* Description     : Gets messages                                           */
/* Notes           : Manipulate the Consumer Pointer                         */
/*                                                                           */
/*---------------------------------------------------------------------------*/
usys     msg_get (usys *src_id, usys *size, void *loc, usys time_out)
{
    msg_box_t *msg_box    ; 
    msg_t     *msg        ;

    tcb_t     *this_task  ;
    usys      stat = GOOD ;

    this_task = (tcb_t *) task_id_get() ;
    msg_box   = this_task->msg_box ;  

    os_pause() ; /* Make access atomic */

    /* Check if the msg box is empty */
    if (msg_box->num_msgs == 0)
    {  /* No Messages...*/

       if (time_out == DONT_WAIT)
       {
	  stat = E_RES ;
	  goto err ;
       }

       /* Handle Timeouts */
       this_task->time_out     = NO ;
       this_task->got_resource = NO ;

       if (time_out != WAIT_FOREVER)
       {  
          /* Activate the Timer */
          this_task->time_count   = time_out ;
          this_task->timer_active = YES ;
       } 

       /* Put the calling task into the TASK_WAITING state */
       this_task->state = TASK_WAITING ;
       task_run_next (this_task) ;

       /* <-- The caller got re-started here */

       /* Check why if it got re-started because the timer expired */
       if ((this_task->got_resource == NO) && (this_task->time_out == YES))
       {
          stat = E_TIMEOUT ;
          goto err ;
       }
    }

    /*---------------------------*/
    /* Start copying the Message */
    /*---------------------------*/

    /* Calculate the Message Location */
    msg = (msg_t *) ( ((usys) msg_box->msg_buf)              +
                      (msg_box->c_index * sizeof (msg_t))    +
                      (msg_box->c_index * msg_box->max_size)
		    ) ;

    *src_id = msg->id   ;
    *size   = msg->size ;

    /* Copy data */
    msg++ ; /* Message Data */
    memcpy (loc, msg, *size) ;

    /* Update the next msg index */
    if (++msg_box->c_index == msg_box->max_msgs)
       msg_box->c_index = 0 ;

    /* Update the number of messages received */
    msg_box->num_msgs-- ;


    /* Get ready to return back to the caller */
err:
    if (kernel.int_mode == NO)
       os_restart() ;

    return stat ;

} /* End of function msg_get() */



/*---------------------------------------------------------------------------*/
/*                                                                           */
/* Function Name   : msg_send                                                */
/* Description     : Sends a message                                         */
/* Notes           : Manipulate the Producer pointer                         */
/*                                                                           */
/*---------------------------------------------------------------------------*/
usys     msg_send (usys dst_id, usys size, void *loc)
{
    msg_box_t *dst_msg_box ;
    msg_t     *msg ;
    tcb_t     *this_task, *dst_task ;
    usys stat = GOOD ;

    /* Check if the receiver's id is valid   */
    dst_task = (tcb_t *) dst_id ;
    if (dst_task->id != dst_id)
       return E_ID ;     

    /* Get the receiver's mBox */
    dst_msg_box = dst_task->msg_box ;

    /* Check if the msg is too big. */
    if (size > dst_msg_box->max_size)
       return E_VALUE ; 

    os_pause() ; /* Make access atomic */

    /* Check if the receiver's msg_box is full */ 
    if (dst_msg_box->max_msgs == dst_msg_box->num_msgs)
    {
       stat = E_RES ;
       goto err ;
    }

    /*---------------------------*/
    /* Start copying the Message */
    /*---------------------------*/

    /* Calculate the location to put the message */
    msg = (msg_t *) ( ((usys) dst_msg_box->msg_buf) +
                      (dst_msg_box->p_index * sizeof (msg_t)) +
                      (dst_msg_box->p_index * dst_msg_box->max_size)
                    ) ;


    this_task = (tcb_t *) task_id_get() ;
    msg->id   = this_task->id ;
    msg->size = size ;

    memcpy (++msg, loc, size) ;

    /* Update the producer index */
    if (++dst_msg_box->p_index == dst_msg_box->max_msgs)
       dst_msg_box->p_index = 0 ; 

    /* Update the number of messages received */
    dst_msg_box->num_msgs++ ;


    /*----------------------------------*/
    /* Handle the receiver task's state */
    /*----------------------------------*/

    dst_task->got_resource = YES ;
    dst_task->state = TASK_READY ; /* Obviously, the receiver is now ready */

    /* Now check if the receiver task is at a higher priority.   */
    /* If it is, then switch to it and run it...                 */

    if (this_task->priority > dst_task->priority)
    {  
       /* An immidiate switch to the destination task is needed. */
      
       this_task->state = TASK_READY ;    
       task_run_next (this_task) ;      

       /* <-- The caller got re-started here */                    
    }

err:
    if (kernel.int_mode == NO)
       os_restart () ;

    return stat ;

} /* End of function msg_send() */



