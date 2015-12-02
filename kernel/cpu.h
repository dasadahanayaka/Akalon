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
/* File Name       : cpu.h                                                   */
/* Description     : Akalon CPU Interface                                    */
/* Notes           :                                                         */
/*                                                                           */
/*---------------------------------------------------------------------------*/
#ifndef  CPU_H
#define  CPU_H

extern void   cpu_task_first      (void *new_stack) ;
extern void   cpu_task_switch     (void *old_stack, void *new_stack) ;
extern void   cpu_int_task_enter  (void *stack_ptr) ;
extern void   cpu_int_task_exit   (void *saveState, void *loadState) ;

extern usys   cpu_int_init        (void) ;
extern void   cpu_disable_ints    (void) ;
extern void   cpu_enable_ints     (void) ;

extern usys   cpu_task_ram_size   (void) ;
extern void   cpu_task_ram_init   (tcb_t *tcb, usys arg0, usys arg1, 
                                   usys arg2) ;

#endif   /* !CPU_H */
