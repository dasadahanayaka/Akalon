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
/* File Name       : pcboot.h                                                */
/* Description     : pcboot Loader defines                                   */
/* Notes           :                                                         */
/*                                                                           */
/*---------------------------------------------------------------------------*/
#ifndef  PCBOOT_H
#define  PCBOOT_H


#define  GOOD                0
#define  BAD                 1

/* The biggest guarenteed area for use on a PC in real-mode is between */
/* 0xfe00 - 0x7ffff which is about 480 KB. Therefore, use this area    */
/* whenever possible before the OS comes-up                            */ 

#define  PCBOOT_LOC          0x7c00
#define  PCBOOT_STACK        0xffff    /* Real Mode Stack */
#define  PCBOOT_SECTORS      7         /* + 1 (given by the BIOS) = 8 */

#define  DISK_XFER_DST       0x10000 

#define  SECTOR_SIZE         512    /* Bytes */
#define  MAX_SECTOR_XFER     127    /* almost at the 64KB Sector Boundary */

#define  LBA_START           8                   /* 4KB Mark */
#define  LBA_END             (LBA_START + 256)   /* 128 KB   */     

#define  OS_START            0x100000            /*  1 MB    */  
#define  OS_END              0x2000000           /* 32 MB    */

#define  PMODE_STACK         0x7ffff   /* Protected Mode Stack */

/* Floppy Disk Defines */
#define  HEADS_PER_DISK      2
#define  TRACKS_PER_HEAD     80
#define  SECTORS_PER_TRACK   18


#ifndef  ASM_FILE

typedef  unsigned  char      u8   ;
typedef            char      s8   ;
typedef  unsigned  short     u16  ;
typedef            short     s16  ;
typedef  unsigned  int       u32  ;
typedef            int       s32  ;
typedef  unsigned  long long u64  ;

typedef  unsigned  int       usys ;
typedef            int       ssys ;

typedef  struct    fdParamT
{
    u8   head      ;
    u8   track     ;
    u8   start     ;
    u8   count     ;
} fdParamT ;

typedef  struct    dapT 
{
    u8   size      ;
    u8   na        ;
    u16  sectors   ;
    u16  offset    ;
    u16  segment   ;
    u64  lbaStart  ;
} dapT   ;

extern   void      hdTransfer     (void) ;
extern   void      fdTransfer     (void) ;
extern   void      cls            (void) ;
extern   int       printf         (const char *format, ...) ;

extern   dapT      dap ;
extern   fdParamT  fdParam ;
extern   u8        srcType ;

#endif   /* !ASM_FILE */

#endif /* !PCBOOT_H */  
