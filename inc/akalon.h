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
/* File Name       : akalon.h                                                */
/* Description     : Akalon's Interface                                      */
/* Notes           :                                                         */
/*                                                                           */
/*---------------------------------------------------------------------------*/
#ifndef  AKALON_H
#define  AKALON_H

#define  AKALON_VERSION      "2014-1013"

#define  NO             0
#define  YES            1

#define  FALSE          0
#define  TRUE           1

#define  NOT_ACTIVE     0
#define  ACTIVE         1

#define  DISABLE        0
#define  ENABLE         1

#define  NULL           ((void *)0)

#define  NAME_SIZE      32

#define  GOOD           0
#define  BAD            1
#define  E_VALUE        2
#define  E_ITEM         3
#define  E_TIMEOUT      4
#define  E_INIT         5
#define  E_ID           6
#define  E_MEM          7
#define  E_RES          8

#define  DONT_WAIT      0
#define  WAIT_FOREVER   -1

#if defined(ia32)
#include "ia32.h"
#endif

#if defined(arm)
#include "arm.h"
#endif


/******************/
/* Task Interface */
/******************/

extern usys   task_new       (usys priority, usys time_slice, usys stack_size,
                              usys max_msgs, usys max_msg_size, 
                              void (*entry_func)(),
                              usys arg0, usys arg1, usys arg2, 
                              char *name, usys *id) ;

extern usys   task_del       (usys id) ;

extern void   task_delay     (usys ticks) ;
extern void   task_sleep     (void) ;
extern usys   task_wake      (usys id) ;

extern usys   task_info      (usys id, usys *priority, usys *time_slice,
                              usys *stack_size, usys *max_msgs,
                              usys *max_msg_size, char *name) ;

extern usys   task_id_get    (void) ;


/*************************/
/* Message Box Interface */
/*************************/
extern usys   msg_get        (usys *src_id, usys *size, void *loc,
                              usys time_out) ;

extern usys   msg_send       (usys dst_id, usys size, void *loc) ;


/***********************/
/* Semaphore Interface */
/***********************/

extern usys   sem_new        (usys count_max, usys count_start, char *name,
                              usys *id) ;

extern usys   sem_del        (usys id) ;

extern usys   sem_get        (usys id, usys timeOut) ;

extern usys   sem_give       (usys id) ;

extern usys   sem_info       (usys id, usys *count_max, usys *count_now, 
                              char *name) ;


/*******************/
/* Timer Interface */
/*******************/

#define  STD_TICS_PER_SEC    100

#define  TIMER_ONESHOT       0
#define  TIMER_REPEAT        1

extern usys   timer_new           (usys tics, usys type, void (*timer_func)(), 
                                   char *name, usys *id) ;

extern usys   timer_del           (usys id) ;

extern usys   timer_start         (usys id) ;

extern usys   timer_stop          (usys id) ;

extern usys   timer_info          (usys id, usys *tics, usys *type, char *name,
                                   usys *tics_now, usys *is_active) ;

extern void   timer_tic           (void) ;  
extern usys   timer_tics_per_sec  (void) ;  

extern usys   jiffy     ;


/***********************/
/* Interrupt Interface */
/***********************/

extern usys   int_config     (usys int_num, usys status, void (*isr)()) ;


/******************/
/* Link Interface */
/******************/

/* Interface Types */
#define  LINK_POLL      1
#define  LINK_BLOCK     2

typedef  struct    link_t 
{
    /* Configuration functions and variables */
    usys (*conf_func)    (usys cmd, void *arg0, void *arg1) ;
    usys (*tx_conf_var)  (usys cmd, void *arg0, void *arg1) ;
    usys (*rx_conf_var)  (usys cmd, void *arg0, void *arg1) ;

    /* Data Transfer Function */
    usys (*rx_func)      (usys arg, usys size, void *data)  ;  /* Implement */
    usys (*tx_func)      (usys arg, usys size, void *data)  ;  /* Filled    */

} link_t ;

extern usys   os_link        (link_t *top, link_t *rx, link_t *tx) ;


/*********************/
/* General Interface */
/*********************/

extern usys   os_init        (usys mem_start, usys mem_size) ;
extern void   os_pause       (void) ;
extern void   os_restart     (void) ;
extern void   bsp_pre_init   (usys mem_start, usys mem_size) ;
extern void   bsp_post_init  (void) ;




/* Non-Kernel Interfaces */

#include "utils.h"
#include "modules.h"
#include "devices.h"
#include "net.h"


#endif   /* ! AKALON_H */
