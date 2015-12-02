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
/* File Name       : stdlib.c                                                */
/* Description     : stdlib.h library functions.                             */
/* Notes           :                                                         */
/*                                                                           */
/*---------------------------------------------------------------------------*/
#include <akalon.h>
#include <stdlib.h>



/*---------------------------------------------------------------------------*/
/*                                                                           */
/* Function Name   : atoi                                                    */
/* Description     :                                                         */
/* Notes           :                                                         */
/*                                                                           */
/*---------------------------------------------------------------------------*/
int      atoi (const char *str)
{
    char *sign   = NULL ;
    char *first  ;
    char *last   ;
    usys abs_val ;
    int  val     ;

    first = (char *) str ;

    /* Get to the first non-white space */
    while ((*first == ' ') && (*first != 0))
       first ++ ;

    /* Make sure the first digit is a numeric or a sign */
    if (((*first < 0x30) || (*first > 0x39)) &&
        (*first != '+')                      && 
        (*first != '-'))
       return 0 ;

    /* Strip the Sign */
    if ((*first == '+') || (*first == '-'))
       sign = first++ ;  

    /* Make sure what follows is a numeric */
    if ((*first < 0x30) || (*first > 0x39))
       return 0 ;

    /* Now find the last numeric */
    last = first ;
    while ((*last >= 0x30) && (*last <= 0x39))
       last++ ;
        
    if (last != first)
       last-- ;

    if (dec_str_to_val (first, last, &abs_val) != GOOD)
       return 0 ;

    val = abs_val ;

    if ((sign != NULL) && (*sign == '-'))
       val *= -1 ;

    return val ; 

} /* End of function atoi() */



/*---------------------------------------------------------------------------*/
/*                                                                           */
/* Function Name   : itoa                                                    */
/* Description     : Returns back a Decimal or Hex String                    */
/* Notes           : Inspiration from Multi-Boot Code. Thanks You !!!        */
/*                                                                           */
/*---------------------------------------------------------------------------*/
char     *itoa (int val, char *str, int base)
{
    usys neg_val = NO ;
    char buf [32] ;
    usys digit ;     
    char *tmp  ;
    usys u_val ;
    char *ret  ;


    /* Handle a Negative Value as a Positive Value and then */
    /* put the negative sign '-' later. This is applicable  */
    /* only to decimal values.                              */

    u_val = val ;
    if (val < 0)
       if (base != 16)
       {
          u_val *= -1 ;
          neg_val = YES ;
       }

    /* Now start building the ascii value */
    tmp = buf ;
    do 
    {
      digit = u_val % base ;

      if (digit < 10)
      {
         *tmp++ = '0' + digit ;
      } else {
         *tmp++ = '7' + digit ; 
      }

    } while (u_val /= base) ;


    /* The ASCII value is now in buf and needs to get xfered */
    /* over to str. But before doing that, check if we need  */
    /* to add a '-' for a decimal value                      */

    ret = str ;
    if ((base == 10) && (neg_val == YES)) 
       *str++ = '-' ;

    while (--tmp >= buf) 
       *str++ = *tmp ;

    /* Null Terminate */
    *str = 0 ;

    return ret ;

} /* End of function itoa() */

