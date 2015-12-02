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
/* File Name       : stdarg.h                                                */
/* Description     : Akalon's stdarg C library Interface                     */
/* Notes           :                                                         */
/*                                                                           */
/*---------------------------------------------------------------------------*/
#ifndef  STDARG_H
#define  STDARG_H

#define  va_start(v,l)       __builtin_va_start(v,l)
#define  va_arg(v,l)         __builtin_va_arg(v,l)
#define  va_end(v)           __builtin_va_end(v)
#define  va_copy(d,s)        __builtin_va_copy(d,s)

typedef  __builtin_va_list   va_list ;


#endif   /* !STDARG_H */
