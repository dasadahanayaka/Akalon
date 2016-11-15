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
/* File Name       : ipv4.c                                                  */
/* Description     : IP-V4 code                                              */
/* Notes           :                                                         */
/*                                                                           */
/*---------------------------------------------------------------------------*/
#include <akalon.h>
#include "net_priv.h"


typedef  struct    ipv4_mgr_t
{
    net_buf_t      *pkt_hold  ;
} ipv4_mgr_t ;

static   ipv4_mgr_t  ipv4_mgr ;  /* Malloc this <-- DO */

/*---------------------------------------------------------------------------*/
/*                                                                           */
/* Function Name   : ipv4_hdr_init                                           */
/* Description     :                                                         */
/* Notes           :                                                         */
/*                                                                           */
/*---------------------------------------------------------------------------*/
void     ipv4_hdr_init (netif_t *netif, ipv4_hdr_t *ipv4_pkt,
                        usys len, usys protocol,u32 dst_ip)
{
    ipv4_pkt->ver_ihl      = 0x45 ;
    ipv4_pkt->diff         = 0    ;
    ipv4_pkt->len          = htons(len)  ;
    ipv4_pkt->id           = htons(netif->ipv4_id) ;
    ipv4_pkt->flags_offset = htons(0) ;
    ipv4_pkt->ttl          = 0xff ;
    ipv4_pkt->protocol     = (u8) protocol  ;
    ipv4_pkt->csum         = 0    ;
    ipv4_pkt->src          = netif->ip_addr ;
    ipv4_pkt->dst          = htonl(dst_ip)  ;
    ipv4_pkt->csum         = ~crc16_calc (ipv4_pkt, 20) ;

    netif->ipv4_id++ ;

} /* End of function ipv4_hdr_init() */



/*---------------------------------------------------------------------------*/
/*                                                                           */
/* Function Name   : ipv4_hdr_init                                           */
/* Description     :                                                         */
/* Notes           :                                                         */
/*                                                                           */
/*---------------------------------------------------------------------------*/
usys     is_fragment (netif_t *netif, net_buf_t *net_buf)
{
    ipv4_hdr_t *ipv4_hdr ;
    u16 flags_offset ;

    ipv4_hdr = (ipv4_hdr_t *) (((usys) net_buf->data) + ETH_HDR_SIZE) ;

    flags_offset = ntohs (ipv4_hdr->flags_offset) ;

    if ((flags_offset & 0x2000) || (flags_offset & 0x1fff))
       NET_DEBUG ("flg 0x%x %d %d\n", ntohs(ipv4_hdr->id), 
                  (flags_offset & 0x2000)>>13, 
                  (flags_offset & 0x1fff) * 8) ;

    return NO ;

} /* End of function is_fragment() */


/*---------------------------------------------------------------------------*/
/*                                                                           */
/* Function Name   : ipv4_pkt_rx                                             */
/* Description     : Handle a received ipv4 packet                           */
/* Notes           :                                                         */
/*                                                                           */
/*---------------------------------------------------------------------------*/
void     ipv4_pkt_rx (netif_t *netif, net_buf_t *net_buf)
{
    ipv4_hdr_t *ipv4_hdr ;

    ipv4_hdr = (ipv4_hdr_t *) (((usys) net_buf->data) + ETH_HDR_SIZE) ;

    /* Check if the packet is for US */
    if (ipv4_hdr->dst != netif->ip_addr)
       return ;

    is_fragment (netif, net_buf) ;


    /* Re-assemble packets */

    switch (ipv4_hdr->protocol)
    {
       case IPV4_PROTOCOL_UDP :
            udp_pkt_rx (netif, net_buf) ;
	    break ;

       case IPV4_PROTOCOL_ICMP :
            icmp_pkt_rx (netif, net_buf) ;
	    break ;

       default :
	    break ;
    }

} /* End of function ipv4_pkt_rx() */



/*---------------------------------------------------------------------------*/
/*                                                                           */
/* Function Name   : ipv4_init                                               */
/* Description     : Initialize the ipv4 mgr                                 */
/* Notes           :                                                         */
/*                                                                           */
/*---------------------------------------------------------------------------*/
usys     ipv4_init (netif_t *netif)
{
    ipv4_mgr.pkt_hold = NULL ;
    netif->ipv4_mgr = &ipv4_mgr ;

    return GOOD ;
} /* End of function ipv4_init() */








