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
/* File Name       : cpu.c                                                   */
/* Description     : General IA32 code                                       */
/* Notes           :                                                         */
/*                                                                           */
/*---------------------------------------------------------------------------*/
#include <akalon.h>
#include <stdio.h>
#include <stdlib.h>

#include "../../kernel/kernel.h"
#include "../../kernel/cpu.h"

#include "cpu_ia32.h"



/*---------------------------------------------------------------------------*/
/*                                                                           */
/* Function Name   : inX                                                     */
/* Description     : Inputs from Ports                                       */
/* Notes           :                                                         */
/*                                                                           */
/*---------------------------------------------------------------------------*/
u8       in8  (u16 port)
{
    u8   val   ;
    __asm__ volatile("inb %1,%0" : "=a" (val) : "dN" (port));
    return val ;
} /* End of function in8 () */

u16      in16 (u16 port)
{
    u16  val   ;
    __asm__ volatile("inw %1,%0" : "=a" (val) : "dN" (port));
    return val ;
} /* End of function in16 () */

u32      in32 (u16 port)
{
    u32  val   ;
    __asm__ volatile("inl %1,%0" : "=a" (val) : "dN" (port));
    return val ;
} /* End of function in32 () */



/*---------------------------------------------------------------------------*/
/*                                                                           */
/* Function Name   : outX                                                    */
/* Description     : Output to Ports                                         */
/* Notes           :                                                         */
/*                                                                           */
/*---------------------------------------------------------------------------*/
void     out8  (u16 port, u8  val)
{
    __asm__ volatile("outb %0,%1" : : "a" (val), "dN" (port));
} /* End of function out8  () */

void     out16 (u16 port, u16 val)
{
    __asm__ volatile("outw %0,%1" : : "a" (val), "dN" (port));
} /* End of function out16 () */

void     out32 (u16 port, u32 val)
{
    __asm__ volatile("outl %0,%1" : : "a" (val), "dN" (port));
} /* End of function out32 () */



/*---------------------------------------------------------------------------*/
/*                                                                           */
/* Function Name   : inX                                                     */
/* Description     : Display IO Port functions                               */
/* Notes           :                                                         */
/*                                                                           */
/*---------------------------------------------------------------------------*/
void     in_8  (u16 port) 
{
    printf ("\n0x%x : 0x%x\n", port, in8 (port)) ;
} /* End of function in8 () */

void     in_16 (u16 port) 
{
    printf ("\n0x%x : 0x%x\n", port, in16 (port)) ;
} /* End of function in_16 () */

void     in_32 (u16 port) 
{
    printf ("\n0x%x : 0x%x\n", port, in32 (port)) ;
} /* End of function in_32 () */



/*---------------------------------------------------------------------------*/
/*                                                                           */
/* Function Name   : cpuid_show                                              */
/* Description     : Display IO Port functions                               */
/* Notes           :                                                         */
/*                                                                           */
/*---------------------------------------------------------------------------*/
void  cpuid_show (void)
{
    u32 eax, ebx, ecx, edx ;

    eax = 0x1 ;
    cpuid (&eax, &ebx, &ecx, &edx) ;

    printf ("\nEAX = 0x%x EBX = 0x%x ECX = 0x%x EDX = 0x%x\n", 
            eax, ebx, ecx, edx) ;
} /* End of function cpuid_show() */
