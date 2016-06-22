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
/* File Name       : kernel.h                                                */
/* Description     : The Kernel defines.                                     */
/* Notes           :                                                         */
/*                                                                           */
/*---------------------------------------------------------------------------*/
#ifndef  KERNEL_H
#define  KERNEL_H

/* Task States */
#define  TASK_RUNNING   1
#define  TASK_READY     2
#define  TASK_WAITING   3
#define  TASK_SLEEPING  4
#define  TASK_DELETE    5


/*----------------------*/
/* Task Control Block   */
/*----------------------*/
typedef  struct    tcb_t     
{
    usys      id               ;
    usys      priority         ;
    usys      stack_size       ;
    usys      time_slice       ;
    usys      max_msgs         ;
    usys      max_msg_size     ; 
    void      (*func)()        ;
    char      name [NAME_SIZE] ;    
    usys      *stack_ptr       ;
    void      *msg_box         ;

    usys      timer_active     ;
    usys      time_count       ;
    usys      time_out         ;

    usys      state            ;
    usys      got_resource     ;

    usys      wait_id          ;

    struct    tcb_t  *pBlk     ;
    struct    tcb_t  *nBlk     ;

} tcb_t ;


/*--------------------------*/
/* Semaphore Control Block  */
/*--------------------------*/
typedef  struct    sem_t
{ 
    usys      id               ;            
    usys      count_max        ;
    usys      count_now        ;
    char      name [NAME_SIZE] ;

    tcb_t     *wait_q          ;

    struct    sem_t  *next     ;

} sem_t ;


/*----------------------*/
/* Timer Control Block  */
/*----------------------*/
typedef  struct    timer_t
{
    usys      id               ;
    usys      tics             ;
    usys      type             ;
    void      (*func)()        ;
    char      name [NAME_SIZE] ;

    usys      active           ;
    usys      tics_now         ;

    struct    timer_t *next    ;
} timer_t ;


/*-------------------------------*/
/* Kernel's Control Block        */
/*-------------------------------*/

typedef  struct    kernel_t
{
    tcb_t     *task_q        ;
    tcb_t     *task_idle     ;

    usys      mem_start      ;
    usys      mem_size       ;
    usys      mem_free_size  ;
    usys      mem_free_loc   ;   

    usys      int_mode       ;  /* ?? */
    usys      max_ints       ;
    usys      int_num        ;
    void      (*usr_isr[256])() ; 

    sem_t     *sems          ;

    timer_t   *timers        ;
    usys      tics_per_sec   ;

} kernel_t ;

extern   kernel_t  kernel    ;


/*----------------------------*/
/* Kernel Function Prototypes */
/*----------------------------*/

extern   usys      mem_init            (usys mem_start, usys size) ;
extern   usys      task_init           (void) ;
extern   usys      int_init            (void) ;
extern   usys      timer_init          (void) ;
extern   usys      sem_init            (void) ;

extern   usys      msgbox_init         (usys max_msgs, usys max_msg_size,
                                        void **msg_box_loc) ; 

extern   void      task_start          (void (*entry)(), usys arg0, usys arg1,
                                        usys arg2) ;
extern   void      task_add_to_q       (tcb_t *task) ;
extern   void      task_rm_from_q      (tcb_t *task) ;

extern   void      task_run_next       (tcb_t *current_task) ;
extern   tcb_t     *task_ready_get     (void) ;
extern   usys      task_awaiting       (usys id, tcb_t **task) ;
extern   void      task_cleanup        (void) ;

#endif  /* !KERNEL_H */
