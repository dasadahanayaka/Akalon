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
/* File Name       : utils.c                                                 */
/* Description     : Utilities                                               */
/* Notes           : OK, there was no other place to put this stuff          */
/*                                                                           */
/*---------------------------------------------------------------------------*/
#include <akalon.h>
#include <string.h>
#include <stdlib.h>


/*---------------------------------------------------------------------------*/
/*                                                                           */
/* Function Name   : skip_spaces                                             */
/* Description     : Skip White Spaces up to the null terminator             */
/* Notes           :                                                         */
/*                                                                           */
/*---------------------------------------------------------------------------*/
void     skip_spaces (u8 *head, u8 **next, u8 **end)
{
    u8   *tmp8 ;

    /* Find the first now-white space */
    tmp8 = head ;

    while ((*tmp8 == ' ') && (*tmp8 != 0))
       tmp8++ ;

    *next = tmp8 ;

    /* Now find the first white space */

    while ((*tmp8 != 0) && (*tmp8 != ' '))
       tmp8 ++ ;

    *end = tmp8 ;

} /* End of function skip_spaces() */



/*---------------------------------------------------------------------------*/
/*                                                                           */
/* Function Name   : dec_str_to_val                                          */
/* Description     : Extracts the value from a Numeric string                */
/* Notes           :                                                         */
/*                                                                           */
/*---------------------------------------------------------------------------*/
usys     dec_str_to_val (char *first, char *last, usys *value)
{
    char *tmp8   ;
    usys i = 1   ;

    /* Initialize Value */
    *value = 0 ;

    /* Start from the End */
    tmp8 = last ;

    do
    {
       /* Error Checking */
       if ((*tmp8 < 0x30) || (*tmp8 > 0x39))
       {
          *value = 0 ;
          return BAD ;
       }

       *value += (*tmp8 - 0x30) * i ;
       i *= 10 ;

    } while (first != tmp8--) ;

    return GOOD ;

} /* End of function dec_str_to_val () */



/*---------------------------------------------------------------------------*/
/*                                                                           */
/* Function Name   : hex_str_to_val                                          */
/* Description     : Extracts the value from a Hex string                    */
/* Notes           :                                                         */
/*                                                                           */
/*---------------------------------------------------------------------------*/
usys     hex_str_to_val (char *first, char *last, usys *value)
{
    usys tmp_val = 0 ;
    usys i = 1 ;
    u8   *tmp8 ;

    /* Start from the Back */
    tmp8 = (u8 *) last  ;

    do
    {
       if ((*tmp8 >= 0x30) && (*tmp8 <= 0x39))
       {  /* Numerics */

          *tmp8 -= 0x30 ;
       } else {
          /* Hex Values */

          switch (*tmp8)
          {
             case 'a':
             case 'A':
                  *tmp8 = 10 ;
                  break ;

             case 'b':
             case 'B':
                  *tmp8 = 11 ;
                  break ;

             case 'c':
             case 'C':
                  *tmp8 = 12 ;
                  break ;

             case 'd':
             case 'D':
                  *tmp8 = 13 ;
                  break ;

             case 'e':
             case 'E':
                  *tmp8 = 14 ;
                  break ;

             case 'f':
             case 'F':
                  *tmp8 = 15 ;
                  break ;

             default:
                  return BAD ;
          }
       }

       tmp_val += *tmp8 * i ;
       i *= 16 ;

    } while ((u8 *) first != tmp8--) ;

    *value = tmp_val ;
    return GOOD ;

} /* End of function hex_str_to_val() */



/*---------------------------------------------------------------------------*/
/*                                                                           */
/* Function Name   : str_to_num                                              */
/* Description     : Returns a value form a hex or digit string              */
/* Notes           :                                                         */
/*                                                                           */
/*---------------------------------------------------------------------------*/
usys     str_to_num (char *str)
{
    usys val ;

    if (( *(str +1) == 'x') || ( *(str +1) == 'X'))
    {
       if (hex_str_to_val (str +2, str + strlen ((char *) str) - 1, &val)
           != GOOD)
          val = 0 ;
    } else {
       val = (usys) atoi (str) ;
    }

    return val ;
} /* End of function str_to_num() */




