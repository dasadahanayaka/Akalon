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
/* File Name       : modules.h                                               */
/* Description     : Akalon's Module Interface                               */
/* Notes           :                                                         */
/*                                                                           */
/*---------------------------------------------------------------------------*/
#ifndef  MODULES_H
#define  MODULES_H

/* CLI Interface */
extern   usys      cli_init   (void) ;

extern   usys      stdio_init (void) ;
extern   link_t    stdio_link ;

#endif   /* !MODULES_H */