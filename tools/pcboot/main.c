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
/* File Name       : main.c                                                  */
/* Description     : Entry Point for PCBOOT.                                 */
/* Notes           :                                                         */
/*                                                                           */
/*---------------------------------------------------------------------------*/
#include "pcboot.h"


/*---------------------------------------------------------------------------*/
/*                                                                           */
/* Function Name   : xferAndCheck                                            */
/* Description     : Trasnfer Data from DISK_XFER_DST and Check it...        */
/* Notes           : size is in Bytes                                        */
/*                                                                           */
/*---------------------------------------------------------------------------*/
usys     xferAndCheck (usys size, usys loc)
{
    usys *src ;
    usys *dst ;
    usys *end ;

    /* First Move the Data from DISK_XFER_DST to loc */
    src = (usys *) DISK_XFER_DST ;
    dst = (usys *) loc ;
    end = (usys *) (loc + size)  ;

    while (dst != end)
      *dst++ = *src++ ;

    /* Now check the xfer */
    src = (usys *) DISK_XFER_DST ;
    dst = (usys *) loc ;
    end = (usys *) (loc + size)  ;

    while (dst != end)
      if (*dst++ != *src++)
         return BAD ;

    return GOOD ;

} /* End of function xferAndCheck() */



/*---------------------------------------------------------------------------*/
/*                                                                           */
/* Function Name   : hdXfer                                                  */
/* Description     : Transfer data from a Hard Disk                          */
/* Notes           :                                                         */
/*                                                                           */
/*---------------------------------------------------------------------------*/
usys     hdXfer (usys sectorStart, usys numSectors)
{
    usys sectorEnd, i ;
    usys *dst ;

    dap.lbaStart = sectorStart ;
    sectorEnd    = sectorStart + numSectors ;

    dst = (usys *) OS_START ;

    for (i = sectorStart; i < sectorEnd; i += MAX_SECTOR_XFER)
    {
       /* Set up the DAP */
       if ((LBA_END - i) >= MAX_SECTOR_XFER)
       {
          dap.sectors = MAX_SECTOR_XFER  ;
       } else {
          dap.sectors = LBA_END - i ;
       }

       /* Transfer the data on disk (sectors) to DISK_XFER_DST which is */
       /* at 0x10000. xfer() will also go back into real mode and after */
       /* it's done, will come back into protected mode.                */
       hdTransfer () ;

       /* Transfer from Disk to DISK_XFER_DST is complete. Now transfer */
       /* this data to the OS location.                                 */

       if (xferAndCheck (dap.sectors * SECTOR_SIZE, (usys) dst) == GOOD)
       {
           dst += (dap.sectors * SECTOR_SIZE) / sizeof (usys) ;
       } else {
          printf ("pcboot: xfer was bad at sector %d !!!\n", dap.lbaStart) ;
          return BAD ;
       }

       dap.lbaStart += dap.sectors ;
    }

    return GOOD ;

} /* End of function hdXfer() */



/*---------------------------------------------------------------------------*/
/*                                                                           */
/* Function Name   : fdXfer                                                  */
/* Description     : Transfer data from a Floppy Disk                        */
/* Notes           :                                                         */
/*                                                                           */
/*---------------------------------------------------------------------------*/
usys     fdXfer (usys sectorStart, usys numSectors)
{
    usys xfered = 0 ;
    usys *dst ;

    /* BIOS int 0x13 sector numbers start with 1 but LBA numbers start   */
    /* with Zero. Since this function is based on LBAs, but we are using */
    /* BIOS int 0x13, we need to add +1 to the starting sector.          */

    sectorStart ++ ;

    /* Setup the initial conditions */
    fdParam.track = sectorStart / (SECTORS_PER_TRACK * HEADS_PER_DISK) ;
    fdParam.head  = (sectorStart / SECTORS_PER_TRACK) % HEADS_PER_DISK ;
    fdParam.start = sectorStart % SECTORS_PER_TRACK ;

    dst = (usys *) OS_START ;


    /* Start the Xfer */
    while (xfered != numSectors)
    {
       if ((numSectors - xfered) >= SECTORS_PER_TRACK)
       {
          fdParam.count = SECTORS_PER_TRACK ;
       } else {
          fdParam.count = numSectors - xfered ;
       }

       /* Adjust the numSecs to be on a track boundary. This is to */
       /* handle an initial condition where the starting sector is */
       /* not on a SECTORS_PER_TRACK boundary.                     */

       if ((sectorStart % SECTORS_PER_TRACK) != 1)
          fdParam.count = SECTORS_PER_TRACK -
                          (sectorStart % SECTORS_PER_TRACK) + 1 ;

       fdTransfer () ;

       /* Transfer from Disk to DISK_XFER_DST is complete. Now transfer */
       /* this data to the OS location.                                 */

       if (xferAndCheck (fdParam.count * SECTOR_SIZE, (usys) dst) == GOOD)
       {
          dst += (fdParam.count * SECTOR_SIZE) / sizeof (usys) ;
       } else {
          printf ("pcboot: xfer was bad at sector %d !!!\n", fdParam.start) ;
          return BAD ;
       }

       xfered      += fdParam.count ;
       sectorStart += fdParam.count ;

       fdParam.track = sectorStart / (SECTORS_PER_TRACK * HEADS_PER_DISK) ;
       fdParam.head  = (sectorStart / SECTORS_PER_TRACK) % HEADS_PER_DISK ;
       fdParam.start = sectorStart % SECTORS_PER_TRACK ;
    }

    return GOOD ;

} /* End of function fdXfer() */



/*---------------------------------------------------------------------------*/
/*                                                                           */
/* Function Name   : cStart                                                  */
/* Description     : Entry point for PCBOOT's C Code                         */
/* Notes           :                                                         */
/*                                                                           */
/*---------------------------------------------------------------------------*/
void     cStart (void)
{
    void (*func)(usys memSize) ;
    usys *dst, stat ;

    /* If we get here, we are in protected mode. */

    cls() ; /* This initializes printf. Call it before calling printf */
    printf ("pcboot: In cStart (Protected Mode)\n") ;

    /* Zero out the SDRAM */
    printf ("pcboot: Cleaning SDRAM...\n") ;

    dst = (usys *) OS_START ;
    while (dst != (usys *) OS_END)
       *dst++ = 0 ;

    printf ("pcboot: Cleaning DONE...\n")  ;

    /* Now start transfer the disk image to RAM */
    if (srcType == 0x80)
    {
       stat = hdXfer (LBA_START, LBA_END) ;
    } else {
       stat = fdXfer (LBA_START, LBA_END) ;
    }

    if (stat != GOOD)
    {
       printf ("pcboot: <ERR> Disk Transfer Failed !!!\n") ;
       while (1) ;
    }

    printf ("pcboot: Xfer was GOOD...\n") ;

    printf ("pcboot: Activating the OS...\n") ;
    func = (void *) OS_START ;
    func(OS_END) ;

} /* End of function cStart() */



