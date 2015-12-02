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
/* File Name       : devices.h                                               */
/* Description     : Akalon's Device Interface                               */
/* Notes           : Might have to split this up someday                     */
/*                                                                           */
/*---------------------------------------------------------------------------*/
#ifndef  DEVICES_H
#define  DEVICES_H

/* NS16550 Serial Device */

#define  PARITY_NONE    0
#define  PARITY_ODD     1
#define  PARITY_EVEN    2
#define  PARITY_MARK    3
#define  PARITY_SPACE   4

typedef  struct    ns16550_t
{
    void (*reg_write) (usys reg, usys val) ;
    usys (*reg_read)  (usys reg) ;
    usys base_addr ;

    usys clk_freq  ;
    usys baud_rate ;
    usys data_bits ;
    usys parity    ;
    usys stop_bits ;
  
} ns16550_t ;

extern   usys      ns16550_init   (ns16550_t *) ;
extern   void      ns16550_isr    (void) ;
extern   link_t    ns16550_link ;

extern   usys      kbd_init       (void)   ;
extern   void      kbd_isr        (u8 key) ;
extern   link_t    kbd_link ;

/* PCI Bus */      
typedef  struct    pci_t
{
    void (*pci_write)   (usys reg, usys  val) ;
    void (*pci_read)    (usys reg, usys *val) ;
} pci_t ;

extern usys   pci_init  (pci_t *) ;
extern void   pci_show  (void) ;

extern u8     pci_r8    (usys bus_num, usys dev_num, usys func_num, usys addr);
extern u16    pci_r16   (usys bus_num, usys dev_num, usys func_num, usys addr);
extern u32    pci_r32   (usys bus_num, usys dev_num, usys func_num, usys addr);
extern void   pci_w8    (usys bus_num, usys dev_num, usys func_num, usys addr, 
                         u8   val) ;
extern void   pci_w16   (usys bus_num, usys dev_num, usys func_num, usys addr, 
                         u16  val) ;
extern void   pci_w32   (usys bus_num, usys dev_num, usys func_num, usys addr, 
                         u32  val) ;

#endif   /* ! DEVICES_H */
