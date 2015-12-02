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
/* File Name       : cpu_ia32.h                                              */
/* Description     : ia32 specific interface.                                */
/* Notes           :                                                         */
/*                                                                           */
/*---------------------------------------------------------------------------*/
#ifndef  CPU_IA32_H
#define  CPU_IA32_H

typedef  struct    GDTDesType
{
    u16  offsetLow  ;
    u16  segSelect  ;
    u8   dWordCnt   ;
    u8   access     ;
    u16  offsetHigh ;

} GDTDesType ;

typedef  struct    idtLocType
{
    u16  limit     ;
    u32  base      ;
} idtLocType ;

extern   void      loadIDT   (void) ;
extern   void      nullISR   (void) ; 

extern   void      int32ISR  (void) ;                 
extern   void      int33ISR  (void) ;                 
extern   void      int36ISR  (void) ;                 

#endif   /* !CPU_IA32_H */
