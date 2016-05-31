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
/* File Name       : keyboard.c                                              */
/* Description     : Keyboard                                                */
/* Notes           :                                                         */
/*                                                                           */
/*---------------------------------------------------------------------------*/
#include <akalon.h>
#include <stdio.h>
#include <string.h>

static   usys shift_key = NO ;
static   usys cap_lock  = NO ;
static   usys cap_count = 0  ;

static   usys task_id ;
static   u8   key ;

link_t   kbd_link ;


/* US LShift = 42, RShift = 54, LCtrl = 29 */
/* This list is incomplete, but it gets the job done */
static   u8  us_shift_off [128] =
{
    0x00, 0x1b, 0x31, 0x32, 0x33, 0x34, 0x35, 0x36,    /*   7 */
    0x37, 0x38, 0x39, 0x30, 0x2d, 0x3d, 0x08, 0x09,    /*  15 */
    0x71, 0x77, 0x65, 0x72, 0x74, 0x79, 0x75, 0x69,    /*  23 */
    0x6f, 0x70, 0x5b, 0x5d, 0x0d, 0x00, 0x61, 0x73,    /*  31 */
    0x64, 0x66, 0x67, 0x68, 0x6a, 0x6b, 0x6c, 0x3b,    /*  39 */
    0x27, 0x60, 0x00, 0x5c, 0x7a, 0x78, 0x63, 0x76,    /*  47 */
    0x62, 0x6e, 0x6d, 0x2c, 0x2e, 0x2f, 0x00, 0x00,    /*  55 */
    0x00, 0x20, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00     /*  63 */
} ;

static   u8  us_shift_on [128] =
{
    0x00, 0x1b, 0x21, 0x40, 0x23, 0x24, 0x25, 0x5e,    /*   7 */
    0x26, 0x2a, 0x28, 0x29, 0x5f, 0x2b, 0x08, 0x09,    /*  15 */
    0x51, 0x57, 0x45, 0x52, 0x54, 0x59, 0x55, 0x49,    /*  23 */
    0x4f, 0x50, 0x7b, 0x7d, 0x0d, 0x00, 0x41, 0x53,    /*  31 */
    0x44, 0x46, 0x47, 0x48, 0x4a, 0x4b, 0x4c, 0x3a,    /*  39 */
    0x22, 0x7e, 0x00, 0x7c, 0x5a, 0x58, 0x43, 0x56,    /*  47 */
    0x42, 0x4e, 0x4d, 0x3c, 0x3e, 0x3f, 0x00, 0x00,    /*  55 */
    0x00, 0x20, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00     /*  63 */
} ;



/*---------------------------------------------------------------------------*/
/*                                                                           */
/* Function Name   : kbd_char_get                                            */
/* Description     : Convert a Key to an ASCII Character                     */
/* Notes           :                                                         */
/*                                                                           */
/*---------------------------------------------------------------------------*/
usys     kbd_char_get (u8 key, u8 *ascii)
{
    switch (key)
    {
        case 58:  /* Cap Lock Keys */

             cap_count ++ ;

             if (cap_count == 2)
             {
                if (cap_lock == YES)
                {
                   cap_lock = NO  ;
                } else {
                   cap_lock = YES ;
                }
                cap_count = 0 ;
             }

             return BAD ;

        case 42:  /* Shift Active  */
             shift_key = YES ;
             return BAD ;

        case 170: /* Shift Release */
             shift_key = NO ;
             return BAD ;

        default :
             if (key > 127)
                return BAD ;
    }

    /* Default to US Keyboard... OK for Now */
    if (shift_key == NO)
    {
       *ascii = us_shift_off [key] ;

       /* No shift Key + Cap Locks On */
       if ((cap_lock == YES) && ((*ascii > 0x60) && (*ascii < 0x7b)))
          *ascii -= 0x20 ; /* Big Letters */

    } else {

       *ascii = us_shift_on  [key] ;

       /* Shift Key On + Cap Locks On */
       if ((cap_lock == YES) && ((*ascii > 0x40) && (*ascii < 0X5b)))
          *ascii += 0x20 ; /* Little Letters */
    }

    return GOOD ;
} /* End of Function kbd_char_get () */



/*---------------------------------------------------------------------------*/
/*                                                                           */
/* Function Name   : kbd_task                                                */
/* Description     : Keyboard Task                                           */
/* Notes           :                                                         */
/*                                                                           */
/*---------------------------------------------------------------------------*/
void     kbd_task (void)
{
    while (1) 
    {
       task_sleep() ;

       if (kbd_link.tx_func != NULL)
          kbd_link.tx_func (0,1,&key) ;
    }

} /* End of function kbd_task() */



/*---------------------------------------------------------------------------*/
/*                                                                           */
/* Function Name   : kbd_isr                                                 */
/* Description     : Keyboard ISR                                            */
/* Notes           :                                                         */
/*                                                                           */
/*---------------------------------------------------------------------------*/
void     kbd_isr (u8 key_press)
{
    /* If the Keypress is valid, give the Semaphore */
    if (kbd_char_get (key_press, &key) == GOOD)
       task_wake (task_id) ;

} /* End of function kbd_isr() */



/*---------------------------------------------------------------------------*/
/*                                                                           */
/* Function Name   : kbd_init                                                */
/* Description     : Initialize the Keyboard.                                */
/* Notes           :                                                         */
/*                                                                           */
/*---------------------------------------------------------------------------*/
usys     kbd_init (void)
{
    usys  stat ;

    /* Initialize the stdio interface structure */
    memset (&kbd_link, 0, sizeof (link_t)) ;

    /* Create the Keyboard Task */
    stat = task_new (1, 0, 1024, 0,0, kbd_task, 0,0,0, "kbd_task", 
                     &task_id) ;
    if (stat != GOOD)
    {
       printf ("ERR: Creating kbd_task. Stat = %d\n", stat) ;
       return BAD ;
    }

    return GOOD ;

} /* End of function kbd_init() */

