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
/* File Name       : icmp.c                                                  */
/* Description     : ICMP Code (i.e. ping code)                              */
/* Notes           :                                                         */
/*                                                                           */
/*---------------------------------------------------------------------------*/
#include <akalon.h>
#include <string.h>
#include <stdio.h>

#include "net_priv.h"

typedef  struct    icmp_hdr_t
{
    u8  type  ;
    u8  code  ;
    u16 csum  ;
    u16 id    ;
    u16 seq   ;
} __attribute__((packed)) icmp_hdr_t ;

#define  ICMP_HDR_SIZE            8

#define  MSG_TYPE_ECHO_REQUEST    8
#define  MSG_TYPE_ECHO_REPLY      0




/*---------------------------------------------------------------------------*/
/*                                                                           */
/* Function Name   : icmp_pkt_rx                                             */
/* Description     : Handle a received icmp pkt                              */
/* Notes           :                                                         */
/*                                                                           */
/*---------------------------------------------------------------------------*/
void     icmp_pkt_rx (net_inst_t *net_inst, void *buf)
{
    icmp_hdr_t *icmp_pkt_in, *icmp_pkt_out ;
    ipv4_hdr_t *ipv4_hdr_in, *ipv4_hdr_out ;
    eth_hdr_t  *eth_hdr_in ;
    pkt_t      *pkt ;

    eth_hdr_in  = (eth_hdr_t *)  ((usys) buf)  ;
    ipv4_hdr_in = (ipv4_hdr_t *) (((usys) buf) + ETH_HDR_SIZE) ;
    icmp_pkt_in = (icmp_hdr_t *) (((usys) buf) + ETH_HDR_SIZE  + 
                                  IPV4_HDR_SIZE) ;

    switch (icmp_pkt_in->type)
    {
        case MSG_TYPE_ECHO_REQUEST :

             /* Alloc the frame */
             if ((pkt = pkt_alloc (net_inst)) == NULL)
             {
                printf ("ERR: pkt_alloc() failed in handle_arp_pkt() !!\n") ;
                return ;
             }

	     /* Fill out the icmp header and data */
             icmp_pkt_out = (icmp_hdr_t *) (((usys) pkt)   +  
                                            ETH_HDR_SIZE   + 
                                            IPV4_HDR_SIZE) ;

	     icmp_pkt_out->type = MSG_TYPE_ECHO_REPLY ;
	     icmp_pkt_out->code = 0 ;
	     icmp_pkt_out->id   = icmp_pkt_in->id  ;
	     icmp_pkt_out->seq  = icmp_pkt_in->seq ;

	     memcpy (++icmp_pkt_out, ++icmp_pkt_in, 
                     ntohs(ipv4_hdr_in->len) - IPV4_HDR_SIZE - ICMP_HDR_SIZE) ;

	     icmp_pkt_out -- ;
	     icmp_pkt_out->csum = ~crc16_calc ((u8 *) icmp_pkt_out, 
                                              ntohs(ipv4_hdr_in->len) - 
                                              IPV4_HDR_SIZE) ;

	     /* Initialize the ip header */
             ipv4_hdr_out = (ipv4_hdr_t *) (((usys) pkt) + ETH_HDR_SIZE) ;

	     ipv4_hdr_init (net_inst, ipv4_hdr_out, 
                            ntohs(ipv4_hdr_in->len),
                            IPV4_TYPE_ICMP, ntohl(ipv4_hdr_in->src)) ;


             /* Initialize the eth header */
             eth_hdr_init (net_inst, pkt, eth_hdr_in->src_mac, ETH_TYPE_IPV4) ;


	     pkt_send (net_inst, pkt, ntohs(ipv4_hdr_in->len) + 
                       ETH_HDR_SIZE) ;
	     pkt_free (net_inst, pkt) ;

	     break ;

        default :
             break ;
    }

} /* End of function icmp_pkt_rx () */



