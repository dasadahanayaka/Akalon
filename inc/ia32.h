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
/* File Name       : ia32.h                                                  */
/* Description     : Akalon's Intel-32 Interface                             */
/* Notes           :                                                         */
/*                                                                           */
/*---------------------------------------------------------------------------*/
#ifndef  IA32_H
#define  IA32_H

#define  CPU_MAX_INTS        256
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

extern   u8        in8       (u16 port) ;
extern   u16       in16      (u16 port) ;
extern   u32       in32      (u16 port) ;

extern   void      out8      (u16 port, u8  data) ;
extern   void      out16     (u16 port, u16 data) ;
extern   void      out32     (u16 port, u32 data) ;
extern   void      cpuid     (u32 *eax, u32 *ebc, u32 *ecx, u32 *edx) ;

#endif   /* !IA32_H */
