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
/* File Name       : bsp_asm.s                                               */
/* Description     : Assembly language code for Akalon's PC-BSP              */
/* Notes           :                                                         */
/*                                                                           */
/*---------------------------------------------------------------------------*/

        .text

        .globl     remapInts
        .globl     enablePcInts
        .globl     sendEOI
        .globl     readKeyPress


/*---------------------------------------------------------------------------*/
/*                                                                           */
/* Function Name   : remapInts                                               */
/* Description     : Remap the Interrupts through the PIC                    */
/* Notes           :                                                         */
/*                                                                           */
/*---------------------------------------------------------------------------*/
remapInts:
        .align  4

        pushl %eax
        mov   $0x11, %al         # Init Command
        outb  %al,   $0x20       # PIC1 Code Reg
        outb  %al,   $0xa0       # PIC1 Code Reg

        mov   $0x20, %al         # New int 0-7 mapping
        outb  %al,   $0x21       # PIC1 Data Reg

        mov   $0x28, %al         # New int 8-15 mapping
        outb  %al,   $0xa1       # PIC2 Data Reg

        mov   $0x04, %al         # Master
        outb  %al,   $0x21       # PIC1 Data Reg

        mov   $0x02, %al         # Slave
        outb  %al,   $0xa1       # PIC2 Data Reg

        mov   $0x01, %al         # 8086 Mode
        outb  %al,   $0x21       # PIC1 Data Reg

        mov   $0x01, %al         # 8086 Mode
        outb  %al,   $0xa1       # PIC2 Data Reg

        mov   $0xff, %al         # PIC1 Int Mask (All Ints Disabled)
        outb  %al,   $0x21       # PIC1 Data Reg

        mov   $0xff, %al         # PIC2 Int Mask (All Ints Disabled)
        out   %al,   $0xa1       # PIC2 Data Reg

        popl  %eax

        ret



/*---------------------------------------------------------------------------*/
/*                                                                           */
/* Function Name   : enablePcInts                                            */
/* Description     : Enable Interrupts on the PIC                            */
/* Notes           :                                                         */
/*                                                                           */
/*---------------------------------------------------------------------------*/
enablePcInts:
        .align  4

        pushl %eax
        mov   $0xec, %al   /* PIC1 Int Mask. Enable Sys Clock, KBD, COM1 */
        outb  %al, $0x21
        popl  %eax

        ret


/*---------------------------------------------------------------------------*/
/*                                                                           */
/* Function Name   : sendEOI                                                 */
/* Description     : Sends the End of Interrupt to the PIC.                  */
/* Notes           :                                                         */
/*                                                                           */
/*---------------------------------------------------------------------------*/
sendEOI:
        .align  4

        pushl %eax
        mov   $0x20, %ax
        outb  %al, $0x20
        popl  %eax

        ret


/*---------------------------------------------------------------------------*/
/*                                                                           */
/* Function Name   : readKeyPress                                            */
/* Description     : Reads a keyboard character.                             */
/* Notes           :                                                         */
/*                                                                           */
/*---------------------------------------------------------------------------*/
readKeyPress:
        inb   $0x60, %al
        ret
