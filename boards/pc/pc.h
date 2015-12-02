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
/* File Name       : pc.h                                                    */
/* Description     : Standard PC BSP interface file.                         */
/* Notes           :                                                         */
/*                                                                           */
/*---------------------------------------------------------------------------*/
#ifndef  PC_H
#define  PC_H

extern   usys      B8000_init     (void) ;
extern   usys      dev_init       (void) ;

extern   void      remapInts      (void) ;
extern   void      enablePcInts   (void) ;
extern   void      sendEOI        (void) ;

extern   void      cls            (void) ;
extern   u8        readKeyPress   (void) ;

extern   link_t    b8000_link ;

#endif  /* ! PC_H */
