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
/* File Name       : pci.c                                                   */
/* Description     : PCI Bus                                                 */
/* Notes           :                                                         */
/*                                                                           */
/*---------------------------------------------------------------------------*/
#include <akalon.h>
#include <stdio.h>
#include <string.h>

static   void  (*read_func)  (usys, usys *) = NULL ;
static   void  (*write_func) (usys, usys)   = NULL ;


/*---------------------------------------------------------------------------*/
/*                                                                           */
/* Function Name   : get_addr                                                */
/* Description     : Build the PCI Address                                   */
/* Notes           : Uses Mechanism 1                                        */
/*                                                                           */
/*---------------------------------------------------------------------------*/
static   u32   get_addr (usys bus_num, usys dev_num, usys func_num, usys addr)
{
    return ((0x1 << 31)               |   /* Enable Bit       */
            ((bus_num  & 0xff) << 16) |   /* 8 Bits = Max 256 */
            ((dev_num  & 0x1f) << 11) |   /* 5 Bits = Max 32  */
            ((func_num & 0x7)  << 8)  |   /* 3 Bits = Max 7   */
            ((addr     & 0xfc))) ;        /* 6 Bits = Max 64  */
} /* End of function get_addr() */



/*---------------------------------------------------------------------------*/
/*                                                                           */
/* Function Name   : pci_rX                                                  */
/* Description     : Functions to READ the PCI Config Space                  */
/* Notes           :                                                         */
/*                                                                           */
/*---------------------------------------------------------------------------*/
u8      pci_r8  (usys bus_num, usys dev_num, usys func_num, usys addr)
{
    usys data ;

    read_func (get_addr (bus_num, dev_num, func_num, addr), &data) ;
    data >>= (addr % 4) * 8 ;

    return ((u8) (data & 0xff)) ;
} /* End of Function pci_r8  () */

u16     pci_r16 (usys bus_num, usys dev_num, usys func_num, usys addr)
{
    usys data ;

    read_func (get_addr (bus_num, dev_num, func_num, addr), &data) ;
    data >>= (addr % 4) * 8 ;

    return ((u16) (data & 0xffff)) ;
} /* End of Function pci_r16 () */

u32     pci_r32 (usys bus_num, usys dev_num, usys func_num, usys addr)
{
    usys data ;
    read_func (get_addr (bus_num, dev_num, func_num, addr), &data) ;
    return ((u32) data) ;
} /* End of Function pci_r32 () */



/*---------------------------------------------------------------------------*/
/*                                                                           */
/* Function Name   : pci_wX                                                  */
/* Description     : Functions to WRITE the PCI Config Space                 */
/* Notes           : These functions are Read Modify Writes except pci_w32   */
/*                                                                           */
/*---------------------------------------------------------------------------*/
void    pci_w8  (usys bus_num, usys dev_num, usys func_num, usys addr,
                 u8   val)
{
    usys data ;
    read_func (get_addr (bus_num, dev_num, func_num, addr), &data) ;

    /* Zero out the correct byte */
    data &= ~(0xff << (addr % 4) * 8) ;

    /* Write the needed value to correct byte */
    data |= val << (addr % 4) * 8 ;

    /* Write back the value */
    write_func (get_addr (bus_num, dev_num, func_num, addr), data) ;

} /* End of Function pci_w8  () */

void    pci_w16 (usys bus_num, usys dev_num, usys func_num, usys addr,
                 u16  val)
{
    usys data ;

    /* Read the Current Value */
    read_func (get_addr (bus_num, dev_num, func_num, addr), &data) ;

    /* Zero out the correct 16 bits */
    data &= ~(0xffff << (addr % 4) * 8) ;

    /* Write the needed value to correct byte */
    data |= val << (addr % 4) * 8 ;

    write_func (get_addr (bus_num, dev_num, func_num, addr), data) ;

} /* End of Function pci_w16 () */

void    pci_w32 (usys bus_num, usys dev_num, usys func_num, usys addr,
                 u32  val)
{
    write_func (get_addr (bus_num, dev_num, func_num, addr), val) ;
} /* End of Function pci_w32 () */



/*---------------------------------------------------------------------------*/
/*                                                                           */
/* Function Name   : pci_init                                                */
/* Description     : Initialize the PCI Bus                                  */
/* Notes           :                                                         */
/*                                                                           */
/*---------------------------------------------------------------------------*/
usys     pci_init (pci_t *data)
{
    if ((data->pci_read == NULL) || (data->pci_write == NULL))
       return BAD ;

    read_func  = data->pci_read  ;
    write_func = data->pci_write ;

    return GOOD ;

} /* End of function pci_init() */



/*---------------------------------------------------------------------------*/
/*                                                                           */
/* Function Name   : pci_show                                                */
/* Description     : Show all the PCI Devices                                */
/* Notes           :                                                         */
/*                                                                           */
/*---------------------------------------------------------------------------*/
void     pci_show (void)
{
    usys i, j, k ;

    printf ("\n") ;

    for (i = 0; i < 256; i++)
       for (j = 0; j < 32; j++)
          if (pci_r16 (i,j,0,0) != 0xffff)
	  {
             printf ("B=%d D=%d F=%d : Vendor = 0x%x Device = 0x%x\n",
                     i,j,0, pci_r16 (i,j,0,0), pci_r16 (i,j,0,2)) ;

	     /* Check for multi-function devices with the header */
	     if (pci_r8 (i,j,0,0xe) & 0x80)
                for (k = 1; k < 8; k++)
                   if (pci_r16 (i,j,k,0) != 0xffff)
                      printf ("B=%d D=%d F=%d : Vendor = 0x%x Device = 0x%x\n",
                              i,j,k,pci_r16 (i,j,k,0), pci_r16 (i,j,k,2)) ;
	  }

} /* End of function pci_show() */



/*---------------------------------------------------------------------------*/
/*                                                                           */
/* Function Name   : pci_dev                                                 */
/* Description     : Show a PCI Device                                       */
/* Notes           :                                                         */
/*                                                                           */
/*---------------------------------------------------------------------------*/
void     pci_dev (usys bus, usys device, usys func)
{
    usys i ;

    if (pci_r16 (bus, device, func, 0) != 0xffff)
    {
       printf ("\n") ;
       printf ("Vendor          = 0x%x\n", pci_r16 (bus, device, func, 0x0)) ;
       printf ("Device          = 0x%x\n", pci_r16 (bus, device, func, 0x2)) ;
       printf ("Command         = 0x%x\n", pci_r16 (bus, device, func, 0x4)) ;
       printf ("Status          = 0x%x\n", pci_r16 (bus, device, func, 0x6)) ;
       printf ("Rev ID          = 0x%x\n", pci_r8  (bus, device, func, 0x8)) ;
       printf ("Class           = 0x%x\n", pci_r8  (bus, device, func, 0xb)) ;
       printf ("Sub Class       = 0x%x\n", pci_r8  (bus, device, func, 0xa)) ;

       printf ("Cache Line Size = 0x%x\n", pci_r8  (bus, device, func, 0xc)) ;

       printf ("Latency Timer   = 0x%x\n", pci_r8  (bus, device, func, 0xd)) ;

       for (i = 0; i < 2; i++)
         printf ("BAR %d           = 0x%x\n", i,
                 pci_r32 (bus, device, func, 0x10 + (i * 4))) ;

       printf ("Interrupt Line  = 0x%x\n", pci_r8  (bus, device, func, 0x3c)) ;
       printf ("Interrupt PIN   = 0x%x\n", pci_r8  (bus, device, func, 0x3d)) ;

    }

} /* End of function pci_dev() */
