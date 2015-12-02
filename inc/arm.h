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
/* File Name       : arm.h                                                   */
/* Description     : Akalon's ARM Interface                                  */
/* Notes           :                                                         */
/*                                                                           */
/*---------------------------------------------------------------------------*/
#ifndef  ARM_H
#define  ARM_H

#define  CPU_MAX_INTS        4   /* IRQ,FIQ,ABT,UND ?? <-- DO */
#define  LITTLE_ENDIAN


typedef  unsigned  char      u8   ;
typedef            char      s8   ;
typedef  unsigned  short     u16  ;
typedef            short     s16  ;
typedef  unsigned  int       u32  ;
typedef            int       s32  ;

typedef  unsigned  int       usys ;
typedef            int       ssys ;

typedef  unsigned  int       size_t ;

#endif   /* !ARM_H */
