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


static   u16  ipv4_id = 0 ;
/*---------------------------------------------------------------------------*/
/*                                                                           */
/* Function Name   : ipv4_hdr_init                                           */
/* Description     :                                                         */
/* Notes           :                                                         */
/*                                                                           */
/*---------------------------------------------------------------------------*/
void     ipv4_hdr_init (net_inst_t *net_inst, ipv4_hdr_t *ipv4_pkt,
                        usys len, usys protocol,u32 dst_ip)
{
/* Clean this UP <-- DO */
    ipv4_pkt->ver_ihl = 0x45 ;
    ipv4_pkt->diff    = 00 ;
    ipv4_pkt->len = htons(len)  ;
    ipv4_pkt->id  = htons(ipv4_id) ;
    ipv4_pkt->flags_offset = htons(0) ;
    ipv4_pkt->ttl = 0xff ;
    ipv4_pkt->protocol = (u8) protocol ;
    ipv4_pkt->csum = 0 ;
    ipv4_pkt->src = net_inst->ip_addr ;
    ipv4_pkt->dst = htonl(dst_ip) ;

    ipv4_pkt->csum = ~crc16_calc (ipv4_pkt, 20) ;

    ipv4_id++ ; /* To get rid of a stupid warning */
} /* End of function ipv4_hdr_init() */



/*---------------------------------------------------------------------------*/
/*                                                                           */
/* Function Name   : ipv4_pkt_rx                                             */
/* Description     : Handle a received ipv4 packet                           */
/* Notes           :                                                         */
/*                                                                           */
/*---------------------------------------------------------------------------*/
void     ipv4_pkt_rx (net_inst_t *net_inst, void *buf)
{
    ipv4_hdr_t *ipv4_hdr ;

    ipv4_hdr = (ipv4_hdr_t *) (((usys) buf) + ETH_HDR_SIZE) ;

    /* Check if the packet is for US */
    if (ipv4_hdr->dst != net_inst->ip_addr)
       return ;

    switch (ipv4_hdr->protocol)
    {
       case IPV4_PROTOCOL_ICMP :
            icmp_pkt_rx (net_inst, buf) ;
	    break ;
 
       default :
	    break ;
    }

} /* End of function ipv4_pkt_rx() */














