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
/* File Name       : start.S                                                 */
/* Description     : pcboot loader entry point                               */
/* Notes           :                                                         */
/*                                                                           */
/*---------------------------------------------------------------------------*/
#define  ASM_FILE
#include "pcboot.h"

        .text
        .code16

        .globl     _start

        .globl     hdTransfer
        .globl     dap

        .globl     fdTransfer
        .globl     fdParam

        .globl     srcType


/*---------------------------------------------------------------------------*/
/*                                                                           */
/* Function Name   : _start                                                  */
/* Description     : Entry point to PCBOOT                                   */
/* Notes           :                                                         */
/*                                                                           */
/*---------------------------------------------------------------------------*/
_start:
        /* We are in Real Mode */
        cli

        xorw       %ax, %ax
        movw       %ax, %ds
        movw       %ax, %ss
        movw       %ax, %es

        /* Set the stacks */
        movw       $PCBOOT_STACK, %bp
        movw       %bp, %sp

        /* Save off the Disk Drive Type. This is provided by the BIOS */
        movl       $srcType, %eax
        movb       %dl, (%eax)

        movw       $(welcome), %si
        call       print

        /* The BIOS loads the first 512 bytes (i.e sector 1) of   */
        /* PCBOOT to 0x7c00. PCBOOT then loads the rest of itself */
        /* to 0x7e00.                                             */

        /* -------------- Read the rest of pcboot --------------- */
        /* ah = 0x2                                               */
        /* al = number of sectors                                 */
        /* ch = Cylinder bits 7-0                                 */
        /* cl = Bits 7-6 = Cylinder upper bits, Bits 5-0 = sector */
        /* dh = head                                              */
        /* dl = drive (=0x80 hd, =0x0 fd)                         */
        /* es:bx = segment:offset destination                     */
        /*                                                        */
        /* For Hard Drives...                                     */
        /*                                                        */
        /* Sectors (6 bits)    = 1 - 63                           */
        /* Heads (8 bits)      = 0 - 255                          */
        /* Cylinders (10 bits) = 0 - 1023                         */
        /*                                                        */
        /* For Floppy Drives...                                   */
        /*                                                        */
        /* Sectors (6 bits)    = 1 - 18                           */
        /* Heads (8 bits)      = 0 - 1                            */
        /* Tracks (10 bits)    = ??                               */
        /* ------------------------------------------------------ */
        movb       $0x02, %ah
        movb       $0x7,  %al      /* Read 7 More Sectors  */
        movw       $0x0002, %cx    /* Cylinder 0, Sector 2 */

        /* No need to set %dl because we didn't touch it */
        movb       $0x00, %dh

        xorw       %bx, %bx
        movw       %bx, %es
        movw       $0x7e00, %bx

        int        $0x13
        jc         errRead


        /* Enable A20 */

a20Enable:
1:      inb        $0x64
        testb      $0x2, %al
        jnz        1b
        movb       $0xd1, %al
        outb       $0x64
2:      inb        $0x64
        testb      $0x2, %al
        jnz        2b
        movb       $0xdf, %al
        outb       $0x60

        movw       $(a20Complete), %si
        call       print

        /* Get into pmode */
        movw       $(prePmode), %si
        call       print
        DATA32     call  intoPmode

errRead:
        movw       $(eSectors), %si
        call       print

stop:   jmp        stop

print:        /* AH = 0x0e AL = Char BH = Page Num BL = forground color */
        movw       $0x0001, %bx
        movb       $0x0e, %ah
1:      lodsb
        cmpb       $0, %al
        je         printDone
        int        $0x10
        jmp        1b
printDone:
        ret

welcome:           .string   "PCBoot: Version 1.0\n\r"
a20Complete:       .string   "PCBoot: Enabled A20\n\r"
prePmode:          .string   "PCBoot: Going into pmode\n\r"
eSectors:          .string   "PCBoot: Read failure\n\r"

srcType:
        .byte      0x80


        /* Boot Signature for BIOS */
        . = _start + 0x1fe
sig:
        .word      0xaa55


intoPmode:
        cli        /* Because the disk reads enable interrupts */

        DATA32     ADDR32    lgdt        gdtDesc

        /* Activate pmode */
        movl       %cr0, %eax
        orl        $0x1, %eax
        movl       %eax, %cr0

        DATA32     ljmp      $0x08, $pmodeCode /* Sets the cs to the Code   */
                                               /* section offset in the GDT */

        .code32
pmodeCode:
        movw       $0x10, %ax
        movw       %ax, %ds
        movw       %ax, %es
        movw       %ax, %fs
        movw       %ax, %gs
        movw       %ax, %ss

        /* Setup stack */
        movl       $PMODE_STACK, %esp

        /* Jump to C code */
        call       cStart

stop2:  jmp        stop2




/*---------------------------------------------------------------------------*/
/*                                                                           */
/* Function Name   : hdTransfer                                              */
/* Description     : Transfer Data from Hart Disk to RAM.                    */
/* Notes           : Transfers happen under Real Mode because BIOS functions */
/*                   are used. Also, the start location on Disk is a Logical */
/*                   Address. Just before returning, this function goes back */
/*                   into Protected Mode.                                    */
/*                                                                           */
/*---------------------------------------------------------------------------*/
hdTransfer:
        pusha   /* Save PMODE Regis */

        movw       $0x20, %ax
        movw       %ax, %ds
        movw       %ax, %es
        movw       %ax, %fs
        movw       %ax, %gs
        movw       %ax, %ss

        ljmp       $0x18, $hdRmode

hdRmode:
        .code16

        movl       %cr0, %eax
        andl       $0xfffffffe, %eax      /* Clear PMODE Bit */
        movl       %eax, %cr0

        DATA32     ljmp        $0, $hdRcode

hdRcode:

        xorl       %eax, %eax

        movw       %ax, %ds
        movw       %ax, %es
        movw       %ax, %fs
        movw       %ax, %gs
        movw       %ax, %ss

        sti

        /* Set-up */
        movw       $dap,  %si
        movb       $0x80, %dl
        movb       $0x42, %ah
        int        $0x13
        jc         errRead

        cli     /* Shut Down Interupts before returning */

        movl       %cr0,  %eax
        orl        $0x01, %eax            /* Enable PMODE Bit */
        movl       %eax,  %cr0

        DATA32     ljmp   $0x08, $hdPcode

        .code32
hdPcode:

        movw       $0x10, %ax
        movw       %ax, %ds
        movw       %ax, %es
        movw       %ax, %fs
        movw       %ax, %gs
        movw       %ax, %ss

        popa       /* Resotor PMODE Regs */
        ret



/*---------------------------------------------------------------------------*/
/*                                                                           */
/* Function Name   : fdTransfer                                              */
/* Description     : Transfer Data from Floppy Disk to RAM.                  */
/* Notes           : Transfers happen under Real Mode because BIOS functions */
/*                   are used. Just before returning, this function goes     */
/*                   back into Protected Mode.                               */
/*                                                                           */
/*---------------------------------------------------------------------------*/
fdTransfer:
        pusha   /* Save PMODE Regis */

        movw       $0x20, %ax
        movw       %ax, %ds
        movw       %ax, %es
        movw       %ax, %fs
        movw       %ax, %gs
        movw       %ax, %ss

        ljmp       $0x18, $fdRmode

fdRmode:
        .code16

        movl       %cr0, %eax
        andl       $0xfffffffe, %eax      /* Clear PMODE Bit */
        movl       %eax, %cr0

        DATA32     ljmp        $0, $fdRcode
fdRcode:

        xorl       %eax, %eax
        movw       %ax, %ds
        movw       %ax, %es
        movw       %ax, %fs
        movw       %ax, %gs
        movw       %ax, %ss

        sti

        movb       $0x2, %ah

        movl       $count, %ebx
        movb       (%ebx), %al

        movl       $track, %ebx
        movb       (%ebx), %ch

        movl       $start, %ebx
        movb       (%ebx), %cl

        movl       $head, %ebx
        movb       (%ebx), %dh
        movb       $0x0, %dl

        movw       $0x1000, %bx
        movw       %bx, %es
        movw       $0x0, %bx

        int        $0x13
        jc         errRead

        cli        /* Shut Down Interrupts before returning */

        movl       %cr0,  %eax
        orl        $0x01, %eax            /* Enable PMODE Bit */
        movl       %eax,  %cr0

        DATA32     ljmp   $0x08, $fdPcode

        .code32
fdPcode:

        movw       $0x10, %ax
        movw       %ax, %ds
        movw       %ax, %es
        movw       %ax, %fs
        movw       %ax, %gs
        movw       %ax, %ss

        popa      /* Resotor PMODE Regs */
        ret




/*---------------------------------------------------------------------------*/
/*                                                                           */
/* Needed Data Structures:                                                   */
/*     - GDT                                                                 */
/*     - GDT Description                                                     */
/*     - DAP for reading LBAs                                                */
/*                                                                           */
/*---------------------------------------------------------------------------*/
        .p2align     2
gdt:
        .word        0, 0    /* Dummy GDT */
        .word        0, 0

        /* Code Segment: 0x00000000 - 0xffffffff    */
        .word        0xffff  /* Limit 15-0               */
        .word        0       /* Base  15-0               */
        .byte        0       /* Base  23-16              */
        .byte        0x9a    /* P:DPL:1:X:C:R:A bits     */
        .byte        0xcf    /* G:D:0:V:Limit 19-16 bits */
        .byte        0x0     /* Base  31-24              */

        /* Data Segment: 0x00000000 - 0xffffffff    */
        .word        0xffff  /* Limit 15-0               */
        .word        0       /* Base  15-0               */
        .byte        0       /* Base  23-16              */
        .byte        0x92    /* P:DPL:1:X:C:R:A bits     */
        .byte        0xcf    /* G:D:0:V:Limit 19-16 bits */
        .byte        0x0     /* Base  31-24              */

        /* 16 bit real mode CS */
        .word        0xFFFF
        .word        0
        .byte        0
        .byte        0x9E
        .byte        0
        .byte        0

        /* 16 bit real mode DS */
        .word        0xFFFF
        .word        0
        .byte        0
        .byte        0x92
        .byte        0
        .byte        0

gdtDesc:
        .word        0x27 /* Why 27 and not 28 ? Because .word can only go */
                          /* upto 65535 but there can be 8192 GDTs which   */
                          /* has a size of be 65536. Intel thus made the   */
                          /* value zero count as 1. Besides the GDT can    */
                          /* never be of size zero.                        */
        .long        gdt

dap:
        .byte        0x10                      /* Size of DAP   */
        .byte        0x00                      /* Reserved      */
        .word        0                         /* Count         */
        .word        (DISK_XFER_DST & 0xffff)  /* Offset        */
         .word       (DISK_XFER_DST >> 4)      /* Segment       */
        .long        0                         /* LBA (L) Start */
        .long        0                         /* LBA (H) Start */

fdParam:
head:
        .byte        0x00
track:
        .byte   0x00
start:
        .byte   0x00
count:
        .byte   0x00



