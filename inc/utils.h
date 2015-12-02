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
/* File Name       : utils.h                                                 */
/* Description     : Akalon's Utilities Interface                            */
/* Notes           :                                                         */
/*                                                                           */
/*---------------------------------------------------------------------------*/
#ifndef  UTILS_H
#define  UTILS_H

extern   void      skip_spaces    (u8 *head, u8 **next, u8 **end) ;
extern   usys      hex_str_to_val (char *first, char *last, usys *value) ;
extern   usys      dec_str_to_val (char *first, char *last, usys *value) ;
extern   usys      str_to_num     (char *str) ;

#endif   /* !UTILS_H */
