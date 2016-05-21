/*---------------------------------------------------------------------------*/
/* Akalon RTOS                                                               */
/* Copyright (c) 2011-2016, Dasa Dahanayaka                                  */
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
/* File Name       : string.c                                                */
/* Description     : string.h library functions.                             */
/* Notes           :                                                         */
/*                                                                           */
/*---------------------------------------------------------------------------*/
#include <akalon.h>


/*---------------------------------------------------------------------------*/
/*                                                                           */
/* Function Name   : memcmp                                                  */
/* Description     :                                                         */
/* Notes           :                                                         */
/*                                                                           */
/*---------------------------------------------------------------------------*/
int      memcmp (const void *ptr1, const void *ptr2, size_t num)
{
   u8    *p1, *p2 ;
   usys  i ;

   p1 = (u8 *) ptr1 ;
   p2 = (u8 *) ptr2 ;

   for (i = 0; i < num; i++)
   {
      if (*p1 == *p2)
      {
	 p1++ ;
	 p2++ ;
	 continue ;
      } else {
	 /* Not equal !!! */
	 if (*p1 > *p2)
	 {
	    return 1  ;
	 } else {
	    return -1 ;
	 }
      }
   }

   return 0 ; /* Is equal */
} /* End of function memcmp() */



/*---------------------------------------------------------------------------*/
/*                                                                           */
/* Function Name   : memcpy                                                  */
/* Description     :                                                         */
/* Notes           :                                                         */
/*                                                                           */
/*---------------------------------------------------------------------------*/
void     *memcpy (void *dst, const void *src, size_t size)
{
    u8   *tmpDst ;

    tmpDst = dst ;

    while (size--)
      *tmpDst++ = *(u8 *)src++ ;

    return dst ;
} /* End of function memcpy() */



/*---------------------------------------------------------------------------*/
/*                                                                           */
/* Function Name   : memset                                                  */
/* Description     :                                                         */
/* Notes           :                                                         */
/*                                                                           */
/*---------------------------------------------------------------------------*/
void     *memset (void *dst, int value, size_t size)
{
    u8   *loc ;
    u8   val  ;

    loc  = dst ;
    val  = (u8) value ;

    while (size--)
      *loc++ = val ;

    return dst ;

} /* End of function memset() */



/*---------------------------------------------------------------------------*/
/*                                                                           */
/* Function Name   : strcpy                                                  */
/* Description     :                                                         */
/* Notes           :                                                         */
/*                                                                           */
/*---------------------------------------------------------------------------*/
char     *strcpy (char *dst, const char *src)
{
    char *tmp = dst ;

    while (*src != 0)
      *dst++ = *src++ ;

    *dst = 0 ;

    return (tmp) ;

} /* End of function strcpy() */



/*---------------------------------------------------------------------------*/
/*                                                                           */
/* Function Name   : strcpy                                                  */
/* Description     :                                                         */
/* Notes           :                                                         */
/*                                                                           */
/*---------------------------------------------------------------------------*/
int      strcmp (const char *str1, const char *str2)
{
    while (*str1 == *str2)
    {
       if (*str1 == 0)
          break ;

       str1 ++ ;
       str2 ++ ;
    }

    return (*str1 - *str2) ;

} /* End of function strcmp() */



/*---------------------------------------------------------------------------*/
/*                                                                           */
/* Function Name   : strlen                                                  */
/* Description     :                                                         */
/* Notes           :                                                         */
/*                                                                           */
/*---------------------------------------------------------------------------*/
size_t   strlen (const char *str)
{
    usys i = 0 ;

    while (*str++ != 0)
       i++ ;

    return i ;

} /* End of function strlen() */



/*---------------------------------------------------------------------------*/
/*                                                                           */
/* Function Name   : strnpy                                                  */
/* Description     :                                                         */
/* Notes           :                                                         */
/*                                                                           */
/*---------------------------------------------------------------------------*/
char     *strncpy (char *dst, const char *src, size_t size)
{
    usys trig = NO ;
    char *tmp ;
    usys i ;

    tmp = dst ;

    for (i = 0; i < size; i++)
    {
       if (trig == NO)
       {
          if (*src == 0)
             trig = YES ;

          *dst++ = *src++ ;

       } else {
          *dst++ = 0 ;
       }
    }

    return (tmp) ;

} /* End of function strncpy() */
