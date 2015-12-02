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
/* File Name       : stdio.c                                                 */
/* Description     : stdio.h library functions.                              */
/* Notes           :                                                         */
/*                                                                           */
/*---------------------------------------------------------------------------*/
#include <akalon.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>



/*---------------------------------------------------------------------------*/
/*                                                                           */
/* Function Name   : printf                                                  */
/* Description     :                                                         */
/* Notes           : Inspiration from Multi-Boot Code. Thank You !!!         */
/*                                                                           */
/*---------------------------------------------------------------------------*/
int      printf (const char *format, ...)
{
    char    *tmp  = NULL ;
    int     count = 0 ;
    char    buf [16]  ;
    va_list args ;
    int     c ;

    /* Initialize the List of Input Params */
    va_start (args, format) ;

    while ((c = *format++) != 0)
      if (c != '%')
      {
         putchar (c) ;
      } else {

         c = *format++ ;

         switch (c)
         {
            case 's':
                 tmp = va_arg (args, char *) ;
                 if (tmp == 0)
                    tmp = "(NULL)" ;

                 break ;

            case 'd':
                 itoa (va_arg (args, usys), buf, 10) ;
                 break ;

            case 'x':
                 itoa (va_arg (args, usys), buf, 16) ;
                 break ;

            case 'c':
            default :
                 putchar (va_arg(args, int)) ;
                 count ++ ;
                 continue ;
         }

         if ((c == 'd') || (c == 'x'))
            tmp = buf ;

         while (*tmp != 0)
         {
            putchar (*tmp++) ;
            count ++ ;
         }
      }

    /* End the Input Param List */
    va_end (args) ;

    return count ;
} /* End of function printf() */

