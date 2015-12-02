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
/* File Name       : stdlib.h                                                */
/* Description     : Akalon's stdlib C library Interface                     */
/* Notes           :                                                         */
/*                                                                           */
/*---------------------------------------------------------------------------*/
#ifndef  STDLIB_H
#define  STDLIB_H

extern   int       atoi     (const char *s) ;
extern   char      *itoa    (int value, char *str, int base) ;

extern   void      *malloc  (size_t size) ;
extern   void      free     (void   *ptr) ;

#endif   /* !STDLIB_H */
