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
/* File Name       : buf.c                                                   */
/* Description     : Buffer management code for the Network Stack            */
/* Notes           :                                                         */
/*                                                                           */
/*---------------------------------------------------------------------------*/
#include <akalon.h>
#include <string.h>

#include "net_priv.h"

#define  PKT_SIZE  128
u8  pkt [PKT_SIZE] ;

/*---------------------------------------------------------------------------*/
/*                                                                           */
/* Function Name   : net_buf_init                                            */
/* Description     :                                                         */
/* Notes           :                                                         */
/*                                                                           */
/*---------------------------------------------------------------------------*/
usys     net_buf_init (netif_t *netif)
{
    return GOOD ;
} /* End of function net_buf_init () */



/*---------------------------------------------------------------------------*/
/*                                                                           */
/* Function Name   : pkt_alloc                                               */
/* Description     :                                                         */
/* Notes           :                                                         */
/*                                                                           */
/*---------------------------------------------------------------------------*/
pkt_t    *pkt_alloc (netif_t *netif)
{
    memset (&pkt, 0, PKT_SIZE) ;
    return ((pkt_t *) &pkt) ;
} /* End of function pkt_alloc () */



/*---------------------------------------------------------------------------*/
/*                                                                           */
/* Function Name   : pkt_free                                                */
/* Description     :                                                         */
/* Notes           :                                                         */
/*                                                                           */
/*---------------------------------------------------------------------------*/
void     pkt_free  (netif_t *netif, pkt_t *pkt)
{
} /* End of function pkt_free () */












