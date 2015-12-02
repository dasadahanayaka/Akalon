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
/* File Name       : string.h                                                */
/* Description     : Akalon's string C library Interface                     */
/* Notes           :                                                         */
/*                                                                           */
/*---------------------------------------------------------------------------*/
#ifndef  STRING_H
#define  STRING_H

extern   void      *memcpy  (void *dst, const void *src, size_t size) ;
extern   void      *memset  (void *dst, int value, size_t size) ;
extern   size_t    strlen   (const char *str) ;
extern   int       strcmp   (const char *str1, const char *str2) ;
extern   char      *strncpy (char *dst, const char *src, size_t size) ;
extern   char      *strcpy  (char *dst, const char *src) ;

#endif   /* !STRING_H */
