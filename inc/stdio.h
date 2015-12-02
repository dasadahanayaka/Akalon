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
/* File Name       : stdio.h                                                 */
/* Description     : Akalon's stdio C library Interface                      */
/* Notes           :                                                         */
/*                                                                           */
/*---------------------------------------------------------------------------*/
#ifndef  STDIO_H
#define  STDIO_H

#define  EOF       -1

extern   int       getchar        (void) ;
extern   int       putchar        (int)  ;
extern   int       dbg_putchar    (int)  ;
extern   int       printf         (const char *format, ...) ;

#endif   /* ! STDIO_H */
