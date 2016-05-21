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
/* File Name       : cli_init.c                                              */
/* Description     : Entry point for the Command Line Interface              */
/* Notes           :                                                         */
/*                                                                           */
/*---------------------------------------------------------------------------*/
#include <akalon.h>
#include <string.h>
#include <stdio.h>
#include <cli.h>

extern   void v()  ;

#define  BUF_SIZE       128
#define  MAX_PARAMS     6


/*---------------------------------------------------------------------------*/
/*                                                                           */
/* Function Name   : parse_line                                              */
/* Description     : Parse the input line                                    */
/* Notes           :                                                         */
/*                                                                           */
/*---------------------------------------------------------------------------*/
usys     parse_line (u8 *buf)
{
    u8     *args [32] , *head, *next, *end ;
    usys   arg   [MAX_PARAMS] ;
    usys   ret,  i = 0 ;
    usys   (*f)() ;
    func_t *func  ;
    var_t  *var   ;

    /* Check for just a Return (ie "Enter") key */
    if (buf[0] == 0)
       return GOOD ;

    head = buf ;
    while (1)
    {
       skip_spaces (head, &next, &end) ;

       args [i] = next ;
       if (*next != 0)
          i++ ;

       if (*end != 0)
       {
          *end++ = 0 ;
          head = end ;
       } else {
          break ;
       }

       if (i == 32)
          break ;
    }

    /* Parse through the sym_funcs[] table and see if it's */
    /* a function. If it is, then call the function...     */

    func = &sym_funcs[0] ;

    while (func->name != NULL)
    {
       if (!strcmp (func->name, (char *) args[0]))
       {
          /* Convert arguments to usys's */
          /* Need to handle ints, chars, etc. <-- DO */

          for (i = 0; i < MAX_PARAMS; i++)
            arg [i] = str_to_num ((char *) args [i+1]) ;

          f = (void *) func->func ;

          ret = f (arg[0], arg[1], arg[2], arg[3], arg[4], arg[5]) ;

	  printf ("\nReturn = %d (0x%x)\n",ret,ret) ; 

          return GOOD ;
       } else {
          func++ ;
       }
    }

    /* If we are here, then it was not a function. Parse   */
    /* through the sym_vars[] table and see if it's a      */
    /* variable. If it is, then print out the address and  */
    /* the value...                                        */

    var = &sym_vars[0] ;

    while (var->name != NULL)
    {
       if (!strcmp (var->name, (char *) args[0]))
       {
	  i = *(usys *) var->addr ;
	  printf ("\nValue = %d (0x%x) Address = 0x%x\n", 
                  i,i, var->addr) ; 
	  return GOOD ;
       } else {
 	  var ++ ;
       }
    }

    return BAD ;

} /* End of function parse_line() */



/*---------------------------------------------------------------------------*/
/*                                                                           */
/* Function Name   : cli_task                                                */
/* Description     : Command Line Interface Task                             */
/* Notes           :                                                         */
/*                                                                           */
/*---------------------------------------------------------------------------*/
void     cli_task (void)
{
    u8   buf  [BUF_SIZE] ;
    usys loc  ;
    u8   tmp8 ;
    int  val  ;

    v () ;
    printf ("$ ") ;

    loc = 0 ;
    while (1)
    {
       val = getchar() ; /* Get a Char from stdin. This is a blocking call. */

       if (val != EOF)
       {
          tmp8 = (u8) val ;

          /* Echo character back if it's a printable character */
          if ((tmp8 != 0xa) && (tmp8 != 0xd))
          {
             putchar ((int) tmp8) ;

             if ((tmp8 != 0x8) && (tmp8 != 0x7f))
             {
                buf [loc] = tmp8 ;

                if (++loc == BUF_SIZE)
                   loc = 0 ;
             } else {
                /* Backspace */

                if (loc != 0)
                   loc -- ;

                buf [loc] = 0 ;
             }

          } else {
             buf [loc] = 0 ;

             if (parse_line (buf) != GOOD)
                printf ("\nCommand not found") ;

             printf ("\n$ ") ;

             loc = 0 ;
          }
       } 
    }

} /* End of Function cli_task () */



/*---------------------------------------------------------------------------*/
/*                                                                           */
/* Function Name   : cli_init                                                */
/* Description     : Command Line Init Routine                               */
/* Notes           :                                                         */
/*                                                                           */
/*---------------------------------------------------------------------------*/
usys     cli_init (void) 
{
    usys stat ;
    usys id ;

    /* Create the Command Line Task...*/
    stat = task_new (10, 0, 8192, 0,0, cli_task, 0,0,0, "cli_task", &id) ;
    if (stat != GOOD)
    {
       printf ("ERR: (COMMAND LINE) Creating cli_task. Stat %d\n", stat) ;
       return BAD ;
    }

    return GOOD ;
} /* End of function cli_init () */



