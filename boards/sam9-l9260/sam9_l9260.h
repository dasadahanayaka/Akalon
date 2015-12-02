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
/* File Name       : sam9_l9260.h                                            */
/* Description     : Olimex sam9-l9260 BSP interface file.                   */
/* Notes           :                                                         */
/*                                                                           */
/*---------------------------------------------------------------------------*/
#ifndef  SAM9_L9260_H
#define  SAM9_L9260_H

extern   usys      dev_init       (void) ;

extern   usys      uart_init      (void) ;
extern   void      uart_isr       (void) ;


#endif  /* ! SAM9_L9260_H */
