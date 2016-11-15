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
/* File Name       : net_debug.c                                             */
/* Description     : Debug code for the Network Stack                        */
/* Notes           :                                                         */
/*                                                                           */
/*---------------------------------------------------------------------------*/
#include <akalon.h>
#include <stdio.h>
#include "net_priv.h"


#if defined(NET_MOD_DEBUG)

/*---------------------------------------------------------------------------*/
/*                                                                           */
/* Function Name   : eth_pkt_dump                                            */
/* Description     : Dump the Ethernet Frame                                 */
/* Notes           : For Debug only                                          */
/*                                                                           */
/*---------------------------------------------------------------------------*/
void     eth_pkt_dump (eth_hdr_t *pkt)
{
    printf ("Ethernet Packet:\n") ;
    printf ("  dst mac    = %x:%x:%x:%x:%x:%x\n", 
            pkt->dst_mac[0], pkt->dst_mac[1], pkt->dst_mac[2],
            pkt->dst_mac[3], pkt->dst_mac[4], pkt->dst_mac[5]) ;
    printf ("  src mac    = %x:%x:%x:%x:%x:%x\n", 
            pkt->src_mac[0], pkt->src_mac[1], pkt->src_mac[2],
            pkt->src_mac[3], pkt->src_mac[4], pkt->src_mac[5]) ;
    printf ("  Ether Type = 0x%x\n", htons(pkt->eth_type)) ; 
} /* End of function eth_pkt_dump() */


/*---------------------------------------------------------------------------*/
/*                                                                           */
/* Function Name   : ipv4_pkt_dump                                           */
/* Description     : Dump ipv4 pkt info                                      */
/* Notes           : For Debug only                                          */
/*                                                                           */
/*---------------------------------------------------------------------------*/
void     ipv4_pkt_dump (ipv4_hdr_t *pkt)
{
    usys  offset ; 

    printf ("IPV4 Packet:\n") ;

    offset = ntohs(pkt->flags_offset) & 0x1ff ;
    printf ("  Offset     = 0x%x (%d)\n", offset, offset *8) ;

    printf ("  dst ip     = 0x%x\n", ntohl(pkt->dst)) ;
} /* End of function ipv4_pkt_dump() */


/*---------------------------------------------------------------------------*/
/*                                                                           */
/* Function Name   : udp_pkt_dump                                            */
/* Description     : Dump udp pkt info                                       */
/* Notes           : For Debug only                                          */
/*                                                                           */
/*---------------------------------------------------------------------------*/
void     udp_pkt_dump (udp_hdr_t *pkt)
{
    printf ("UDP Packet:\n") ;
    printf ("  src port   = %d\n", ntohs(pkt->src_port)) ;
    printf ("  dst port   = %d\n", ntohs(pkt->dst_port)) ;
    printf ("  length     = %d\n", ntohs(pkt->len)) ;
} /* End of function udp_pkt_dump() */



/*---------------------------------------------------------------------------*/
/*                                                                           */
/* Function Name   : net_buf_dump                                            */
/* Description     :                                                         */
/* Notes           :                                                         */
/*                                                                           */
/*---------------------------------------------------------------------------*/
void     net_buf_dump (net_buf_t *net_buf)
{
    eth_hdr_t  *eth_hdr  ;
    ipv4_hdr_t *ipv4_hdr ;
    udp_hdr_t  *udp_hdr  ; 

    printf ("\nNet Buffer...\n") ;

    eth_hdr  = (eth_hdr_t *) net_buf->data ;
    ipv4_hdr = (ipv4_hdr_t *) ( ((usys) net_buf->data) + ETH_HDR_SIZE) ;

    udp_hdr  = (udp_hdr_t *) ( ((usys) net_buf->data) +
                                 ETH_HDR_SIZE  + 
                                 IPV4_HDR_SIZE) ;

    eth_pkt_dump  (eth_hdr)  ;
    ipv4_pkt_dump (ipv4_hdr) ;
    udp_pkt_dump  (udp_hdr)  ;

#if 0
    net_buf_t  *tmp_buf  ;
    usys buf_num ;

    tmp_buf  = net_buf ;
    buf_num  = 0 ;

    while (tmp_buf != NULL)
    {
       printf("Net Buf = %d\n", buf_num++) ;
       printf("  Size  = %d\n", tmp_buf->size) ;

       tmp_buf = tmp_buf->next ;
    }
#endif

} /* End of function net_buf_dump () */

#endif /* defined(NET_MOD_DUMP) */



