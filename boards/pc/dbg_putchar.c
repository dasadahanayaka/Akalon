/* kernel.c - the C part of the kernel */
/* Copyright (C) 1999  Free Software Foundation, Inc.

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
   Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.  */

#include <akalon.h>
#include <string.h>
#include <stdlib.h>

/* Macros.  */

/* Check if the bit BIT in FLAGS is set.  */
#define CHECK_FLAG(flags,bit)        ((flags) & (1 << (bit)))

/* Some screen stuff.  */
/* The number of columns.  */
#define COLUMNS                        80
/* The number of lines.  */
#define LINES                        25
/* The attribute of an character.  */
#define ATTRIBUTE   7
/* The video memory address.  */
#define VIDEO                        0xB8000

/* Variables.  */
/* Save the X position.  */
static int xpos ;
/* Save the Y position.  */
static int ypos;
/* Point to the video memory.  */
static volatile unsigned char *video;


link_t    b8000_link ;


/* Clear the screen and initialize VIDEO, XPOS and YPOS.  */
void cls (void)
{
  int i;

  video = (unsigned char *) VIDEO;

  for (i = 0; i < COLUMNS * LINES * 2; i++)
    *(video + i) = 0;

  xpos = 0;
  ypos = 0;
}


/* Put the character C on the screen.  */
void dbg_putchar (int c)
{
    /* Take out the cursor (i.e '_') */
    *(video + (xpos + ypos * COLUMNS) * 2) = 0x00 & 0xFF;
    *(video + (xpos + ypos * COLUMNS) * 2 + 1) = ATTRIBUTE;

    if (c == '\r') 
       return ;

    if ((c != '\n') && (c != 0x8))
    {
       *(video + (xpos + ypos * COLUMNS) * 2) = c & 0xFF;
       *(video + (xpos + ypos * COLUMNS) * 2 + 1) = ATTRIBUTE;
    } else {
       if (c != 0x8)
          xpos = COLUMNS ;
    }

    if (c != 0x8)
    {
       if (++xpos >= COLUMNS)
       {
          xpos = 0 ;

          if (++ypos >= LINES)
	     ypos = 0 ;
       }
    } else {
       /* Backspace */
       if (--xpos < 0)
       {
	  xpos = COLUMNS - 1 ;
          if (--ypos < 0)
	     ypos = 0 ;
       }
    }

    /* Put back the cursor (i.e. '_') */
    *(video + (xpos + ypos * COLUMNS) * 2) = 0x5f & 0xFF;
    *(video + (xpos + ypos * COLUMNS) * 2 + 1) = ATTRIBUTE;

} /* End of function dbg_putchar() */


/*---------------------------------------------------------------------------*/
/*                                                                           */
/* Function Name   : ui_rx                                                   */
/* Description     : Internal RxP function for Upper Modules                 */
/* Notes           :                                                         */
/*                                                                           */
/*---------------------------------------------------------------------------*/
static   usys      rx_func (usys arg, usys size, void *buf)
{
    u8 *tmp8 ;
    usys i ;

    tmp8 = (u8 *) buf ;

    for (i = size; i > 0; i--)
      dbg_putchar ((int) *tmp8++) ;

    return GOOD ;
} /* End of function rx_func() */



/*---------------------------------------------------------------------------*/
/*                                                                           */
/* Function Name   : B8000_init                                              */
/* Description     : Init routine for the b8000 module                       */
/* Notes           :                                                         */
/*                                                                           */
/*---------------------------------------------------------------------------*/
usys     B8000_init (void)
{
    /* Clear the Screen */
    cls() ;

    /* Initialize the stdio interface structure */
    memset (&b8000_link, 0, sizeof (link_t)) ;
    b8000_link.rx_func = rx_func ;

    return GOOD ;

} /* End of Function B8000_init() */
