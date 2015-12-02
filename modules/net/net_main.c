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
/* File Name       : net_main.c                                              */
/* Description     : Network Stack                                           */
/* Notes           :                                                         */
/*                                                                           */
/*---------------------------------------------------------------------------*/
#include <akalon.h>
#include <stdio.h>
#include <string.h>

#include "net_priv.h"



/*---------------------------------------------------------------------------*/
/*                                                                           */
/* Function Name   : net_task                                                */
/* Description     : Entry Point for the Network Task                        */
/* Notes           :                                                         */
/*                                                                           */
/*---------------------------------------------------------------------------*/
void     net_task (void)
{
    printf ("ERR: net_task() is not Implemented !!!\n") ;

    while (1)
      task_delay (-1) ;
} /* End of Function net_task () */



/*---------------------------------------------------------------------------*/
/*                                                                           */
/* Function Name   : net_init                                                */
/* Description     : Initializes the Network Stack                           */
/* Notes           :                                                         */
/*                                                                           */
/*---------------------------------------------------------------------------*/
usys     net_init (void *data)
{
    usys stat ;
    usys id ;

    /* Initialize the Network Buffers */
    if ((stat = net_buf_init()) != GOOD)
    {
       printf("ERR: (NET) net_buf_init(). Stat %d\n", stat) ;
       goto err ;
    }

    /* Initialize Ethernet */
    if ((stat = ether_init()) != GOOD)
    {
       printf("ERR: (NET) ether_init(). Stat %d\n", stat) ;
       goto err ;
    }

    /* Initialize IPV4 */
    if ((stat = ipv4_init()) != GOOD)
    {
       printf("ERR: (NET) ipv4_init(). Stat %d\n", stat) ;
       goto err ;
    }

    /* Initialize UDP */
    if ((stat = udp_init()) != GOOD)
    {
       printf("ERR: (NET) udp_init(). Stat %d\n", stat) ;
       goto err ;
    }

    /* Initialize TCP */
    if ((stat = tcp_init()) != GOOD)
    {
       printf("ERR: (NET) tcp_init(). Stat %d\n", stat) ;
       goto err ;
    }

    /* Create the Network Input Task...*/
    stat = task_new (1, 0, 8192, 0,0, net_task, 0,0,0, "net_task", &id) ;
    if (stat != GOOD)
    {
       printf("ERR: (NET) Creating net_task. Stat %d\n", stat) ;
       return BAD ;
    } 

    return GOOD ;

 err:
    /* Release Buffers <-- DO */

    return BAD ; 
} /* End of function net_init() */



