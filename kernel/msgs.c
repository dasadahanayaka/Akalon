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
    usys id        ;
    usys size      ;
    void *loc      ;
} msg_t ;

typedef  struct    mbox_t
{
    usys  max_msgs  ;
    usys  max_size  ;
    usys  pIndex    ;     /* Producer */
    usys  cIndex    ;     /* Consumer */
    usys  pHaulted  ;
    msg_t *msgs     ;
} mbox_t ;



/*---------------------------------------------------------------------------*/
/*                                                                           */
/* Function Name   : msgbox_init                                             */
/* Description     : Allocate and initialize a message box                   */
/* Notes           :                                                         */
/*                                                                           */
/*---------------------------------------------------------------------------*/
usys     msgbox_init (usys max_msgs, usys max_size, void **msg_box_loc)
{
    mbox_t *mbox = NULL ;
    msg_t  *msgs = NULL ;
    u8     *data = NULL ; 
    usys   i ;

    /* Alloc memory for the mbox_t, msgs_t's and the data */
    mbox = malloc (sizeof(mbox_t)) ;
    if (mbox == NULL)
       return E_MEM ;

    if (max_msgs != 0)
    {
       msgs = malloc (sizeof (msg_t) * max_msgs) ;
       if (msgs == NULL)
          return E_MEM ;

       data = malloc (max_size * max_msgs) ;
       if (data == NULL)
          return E_MEM ;
    }

    mbox->msgs = msgs ;

    for (i = 0; i < max_msgs; i++)
    {
       mbox->msgs[i].loc = data ;
       data += max_size ;
    }

    /* Initialize */
    mbox->max_msgs  = max_msgs ;
    mbox->max_size  = max_size ;
    mbox->pIndex    = 0 ;
    mbox->cIndex    = 0 ;
    mbox->pHaulted  = NO ;

    /* Send back the location */
    *msg_box_loc = mbox ;

    return GOOD ;

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
    tcb_t  *this_task  ;
    usys   stat = GOOD ;
    mbox_t *msg_box ; 

    this_task = (tcb_t *) task_id_get() ;
    msg_box   = this_task->msg_box ;  

    os_pause() ;

    /* Check if mBox is empty. If it is, then depending on the  */
    /* timeOut, take appropriate action.                        */

    if ((msg_box->pIndex == msg_box->cIndex) && (msg_box->pHaulted == NO))
    {  /* There are no messages... */  

       if (time_out == DONT_WAIT)
       {
          stat = E_RES ;
          goto err ;
       }

       /* Handle the time out...*/

       this_task->time_out = NO ;

       if (time_out != WAIT_FOREVER)
       {
          /* Activate the Timer */

          this_task->time_count   = time_out ;
          this_task->timer_active = YES ;
       } 

       this_task->got_resource = NO ;

       /* Mark the calling task as "waiting" and run the next task */

       this_task->state = TASK_WAITING ;
       task_run_next (this_task) ;

       /* Check if we returned because of a timeout */
       if ((this_task->got_resource == NO) && (this_task->time_out == YES))
       {
          stat = E_TIMEOUT ;
          goto err ;
       }
    }

    /* Copy the message for the consumer */

    *src_id = msg_box->msgs [msg_box->cIndex].id   ;
    *size   = msg_box->msgs [msg_box->cIndex].size ;

    memcpy (loc, msg_box->msgs [msg_box->cIndex].loc, 
            msg_box->msgs [msg_box->cIndex].size) ;

    /* Update the Consumer Index */

    msg_box->cIndex ++  ;
    if (msg_box->cIndex == msg_box->max_msgs)
       msg_box->cIndex = 0 ;

    /* Unblock the producer if needed */

    if (msg_box->pHaulted == YES)
       msg_box->pHaulted = NO ;

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
    tcb_t   *dst_task, *this_task ;
    mbox_t  *dst_msg_box ;
    usys    pIndex ;  

    /* Check if the receiver's id is valid   */
    dst_task = (tcb_t *) dst_id ;
    if (dst_task->id != dst_id)
       return E_ID ;     

    /* Get the receiver's mBox */
    dst_msg_box = dst_task->msg_box ;

    /* Check if the msg is too big. */
    if (size > dst_msg_box->max_size)
       return E_VALUE ; 

    os_pause() ;

    /* Check if the receiver's mBox is full */ 
    if (dst_msg_box->pHaulted == YES)
    {
       if (kernel.int_mode == NO)
          os_restart () ;
        
       return E_RES ;
    }

    pIndex = dst_msg_box->pIndex ;

    /* Copy the Message from the producer */

    this_task = (tcb_t *) task_id_get() ;

    dst_msg_box->msgs [pIndex].id   = this_task->id ;
    dst_msg_box->msgs [pIndex].size = size ;    
    memcpy (dst_msg_box->msgs [pIndex].loc, loc, size) ;

    /* Update the Producer Index */

    dst_msg_box->pIndex ++ ;
    if (dst_msg_box->pIndex == dst_msg_box->max_msgs)
       dst_msg_box->pIndex  = 0 ;

    /* If the new index points to the consumer index, */
    /* no more messages can be received.              */
    
    if (dst_msg_box->pIndex == dst_msg_box->cIndex)
       dst_msg_box->pHaulted = YES ;
 
    /* Obviously, the receiver is ready to */
    /* run because we just sent a message. */

    dst_task->got_resource = YES ;
    dst_task->state = TASK_READY ;

    /* Now check if the receiver needs to be run because */
    /* it's priority is higher than the sender.          */

    if (this_task->priority > dst_task->priority)
    {  
       /* An immidiate switch is receiver is needed. Make */
       /* the sender's state "ready" from "running"       */
      
       this_task->state = TASK_READY ;    
       task_run_next (this_task) ;                          
    }

    if (kernel.int_mode == NO)
       os_restart () ;

    return GOOD ;

} /* End of function msg_send() */



