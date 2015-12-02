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
/* File Name       : std_cmds.c                                              */
/* Description     : Standard Akaon Commands                                 */
/* Notes           :                                                         */
/*                                                                           */
/*---------------------------------------------------------------------------*/
#include <akalon.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <cli.h>



/*---------------------------------------------------------------------------*/
/*                                                                           */
/* Function Name   : c                                                       */
/* Description     : Show the Copyright header                               */
/* Notes           :                                                         */
/*                                                                           */
/*---------------------------------------------------------------------------*/
void     c (void)
{
    printf ("\n") ;
    printf ("---------------------------------------------------\n") ;
    printf ("- Akalon RTOS                                     -\n") ;
    printf ("- Copyright (C) 2011-2015, Dasa Dahanayaka        -\n") ;
    printf ("- All Rights Reserved.                            -\n") ;
    printf ("-                                                 -\n") ;
    printf ("- Usage of the works is permitted provided that   -\n") ;
    printf ("- this instrument is retained with the works, so  -\n") ;
    printf ("- that any entity that uses the works is notified -\n") ;
    printf ("- of this instrument.                             -\n") ;
    printf ("-                                                 -\n") ;
    printf ("- DISCLAIMER: THE WORKS ARE WITHOUT WARRANTY.     -\n") ;
    printf ("---------------------------------------------------\n") ;
} /* End of function c() */



/*---------------------------------------------------------------------------*/
/*                                                                           */
/* Function Name   : v                                                       */
/* Description     : Show Version Number                                     */
/* Notes           :                                                         */
/*                                                                           */
/*---------------------------------------------------------------------------*/
void     v (void)
{
    printf ("\n") ;
    printf ("===================================================\n") ;
    printf ("- Akalon RTOS                                     -\n") ;
    printf ("- Version : %s                             -\n",
            AKALON_VERSION) ;
    printf ("- Built   : %s @ %s                -\n",
            __DATE__,__TIME__) ;
    printf ("===================================================\n") ;

} /* End of function v() */



/*---------------------------------------------------------------------------*/
/*                                                                           */
/* Function Name   : r                                                       */
/* Description     : Read Address                                            */
/* Notes           :                                                         */
/*                                                                           */
/*---------------------------------------------------------------------------*/
void     r (usys loc, usys count)
{
    u32  *addr ;
    usys i ;

    /* Get Values */
    addr = (u32 *) loc ;

    if (count == 0)
       count = 4 ;

    printf ("\n") ;
    for (i = 0; i < count; i += sizeof (usys))
      printf ("0x%x: 0x%x 0x%x 0x%x 0x%x\n", (usys) addr + 
              (i * sizeof(usys)) ,
              addr[i], addr[i+1], addr[i+2], addr[i+3]) ;

} /* End of function r () */



/*---------------------------------------------------------------------------*/
/*                                                                           */
/* Function Name   : w                                                       */
/* Description     : Write                                                   */
/* Notes           :                                                         */
/*                                                                           */
/*---------------------------------------------------------------------------*/
void     w (usys loc, usys val, usys size)
{
    u8   *addr8  ;
    u16  *addr16 ;
    u32  *addr32 ;

    if (size == 0)
       size = 4 ;

    switch (size)
    {
       case 1 :
            addr8    = (u8 *) loc ;
            *addr8   = (u8) val ;
            break ;

       case 2 :
            addr16   = (u16 *) loc ;
            *addr16  = (u16) val ;
            break ;

       case 4 :
            addr32   = (u32 *) loc ;
            *addr32  = (u32) val ;
            break ;
    }

} /* End of function w () */






