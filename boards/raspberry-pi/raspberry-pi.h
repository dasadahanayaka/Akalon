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
/* File Name       : raspberry-pi.h                                          */
/* Description     : Raspberry Pi interface file.                            */
/* Notes           :                                                         */
/*                                                                           */
/*---------------------------------------------------------------------------*/
#ifndef  RASPBERRY_PI_H
#define  RASPBERRY_PI_H

extern   usys      dev_init       (void) ;

extern   usys      uart_init      (void) ;
extern   void      uart_isr       (void) ;


/* The Raspberry-PI B+ has 512MB of memory, but only 256 MB can be */
/* used for code because the rest is used for the GPU.             */
#define  BSP_MEM_TOP    0x10000000

#endif  /* ! RASPBERRY_PI_H */
