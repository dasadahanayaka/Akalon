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
/* File Name       : raspberry-pi.ld                                         */
/* Description     : Raspberry Pi linker script.                             */
/* Notes           :                                                         */
/*                                                                           */
/*---------------------------------------------------------------------------*/

SECTIONS
{
   . = 0x8000; 
   . = ALIGN(4);
   .text : 
   {
     *(.text) 
   }

   . = ALIGN(4);
   .data : 
   { 
     *(.data) 
   }

   bss_start = . ;
   . = ALIGN(4);
   .bss  : 
   { 
     *(.bss)  
   }
   bss_end = . ;
}
