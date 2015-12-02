/* boot.S - bootstrap the kernel */
/* Copyright (C) 1999, 2001  Free Software Foundation, Inc.

   This program is free software; you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation; either version 2 of the License, or
   (at your option) any later version.
 
   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.
 
   You should have received a copy of the GNU General Public License
   along with this program; if not, write to the Free Software
   Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA. */

#define ASM     1
#include <multiboot.h>
        
        .text

        .globl  start, _start
start:
_start:
        jmp     multiboot_entry

        /* Align 32 bits boundary. */
        .align  4
        
        /* Multiboot header. */
multiboot_header:
        /* magic */
        .long   MULTIBOOT_HEADER_MAGIC
        /* flags */
        .long   MULTIBOOT_HEADER_FLAGS
        /* checksum */
        .long   -(MULTIBOOT_HEADER_MAGIC + MULTIBOOT_HEADER_FLAGS)

        /* The following is needed to boot a binary file in grub */ 
        /* header_addr */
        .long   multiboot_header
        /* load_addr */
        .long   _start
        /* load_end_addr */
        .long   _edata
        /* bss_end_addr */
        .long   _end
        /* entry_addr */
        .long   multiboot_entry

multiboot_entry:
	lgdt      gdtDesc

        ljmp      $0x08, $pmodeCode /* Sets the cs to the Code   */
                                    /* section offset in the GDT */
.code32
pmodeCode:
        movw       $0x10, %cx
        movw       %cx, %ds
        movw       %cx, %es
        movw       %cx, %fs
        movw       %cx, %gs
        movw       %cx, %ss
	
        /* Initialize the stack pointer. */
        movl    $(stack + STACK_SIZE), %esp

        /* Reset EFLAGS. */
        pushl   $0
        popf
	
        /* Push the pointer to the Multiboot information structure. */
        pushl   %ebx
        /* Push the magic value. */
        pushl   %eax

        /* Now enter the C main function... */
        call    EXT_C(cmain)

        /* Halt. */
        pushl   $halt_message
        call    EXT_C(printf)
        
loop:   hlt
        jmp     loop

halt_message:
        .asciz  "Halted."


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

        /* Our stack area. */
        .comm   stack, STACK_SIZE

	
	