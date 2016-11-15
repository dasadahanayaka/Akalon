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
/* File Name       : udp.c                                                   */
/* Description     : UDP Code                                                */
/* Notes           :                                                         */
/*                                                                           */
/*---------------------------------------------------------------------------*/
#include <akalon.h>
#include <string.h>
#include "net_priv.h"




/*---------------------------------------------------------------------------*/
/*                                                                           */
/* Function Name   : udp_pkt_rx                                              */
/* Description     : UDP Pkt -> Network Stack                                */
/* Notes           :                                                         */
/*                                                                           */
/*---------------------------------------------------------------------------*/
void     udp_pkt_rx (netif_t *netif, net_buf_t *net_buf) 
{
    eth_hdr_t  *eth_hdr  ;
    ipv4_hdr_t *ipv4_hdr ;

    eth_hdr   = (eth_hdr_t *)   ((usys) net_buf->data) ;
    ipv4_hdr  = (ipv4_hdr_t *) (((usys) net_buf->data) + ETH_HDR_SIZE) ;

    NET_BUF_DUMP  (net_buf)  ;
    ETH_PKT_DUMP  (eth_hdr)  ;
    IPV4_PKT_DUMP (ipv4_hdr) ;

} /* End of function udp_pkt_rx () */


#if 0
#define  SET_DST(loc,src)    (*loc = src)
#define  SET_SRC(loc,src)    (*(loc + 2) = src)
#define  SET_LEN(loc,src)    (*(loc + 4) = src)


#define  MAX_UDP_PORTS       2048
#define  MAX_IP_HEADER_SIZE     40 /* Offsets within bufST */

typedef  struct    udpPort 
{
    void (*rtnFunc) (u16 size, usys *data, u16 srcPort, u32 srcIp) ;  
    u32  ipSrc     ;
} udpPort   ;


typedef  struct    udpCoreST 
{
    udpPort udpPorts [MAX_UDP_PORTS] ;
} udpCoreST ;


extern   udpCoreST      udpCoreSD ;


/*---------------------------------------------------------------------------*/
/*                                                                           */
/* Function Name   : udp_port_bind                                           */
/* Description     : Bind and configure a udp port                           */
/* Notes           :                                                         */
/*                                                                           */
/*---------------------------------------------------------------------------*/
usys     udp_port_bind (void (*rtnFunc) (u16 size, usys *data, u32 srcIp, 
                                       u16 port), 
                      u16 port, u32 srcIp)
{
    /* Error Checking */
    if ((port > MAX_UDP_PORTS) || (rtnFunc == NULL))
       return E_VALUE ;
 
    udpCoreSD.udpPorts [port].rtnFunc = NULL ;
    udpCoreSD.udpPorts [port].ipSrc   = 0 ;

    return GOOD ;
} /* End of function udp_port_bind() */



/*---------------------------------------------------------------------------*/
/*                                                                           */
/* Function Name   : udp_port_rel                                            */
/* Description     : Release and reset a udp Port                            */
/* Notes           :                                                         */
/*                                                                           */
/*---------------------------------------------------------------------------*/
usys     udp_port_rel (u16 portNum)
{
    /* Error Checking */
    if (portNum > MAX_UDP_PORTS)
       return E_VALUE ;

    udpCoreSD.udpPorts [portNum].rtnFunc = NULL ;
    udpCoreSD.udpPorts [portNum].ipSrc   = 0 ;
    
    return GOOD ;
} /* End of function udp_port_rel () */



/*---------------------------------------------------------------------------*/
/*                                                                           */
/* Function Name   : udp_send                                                */
/* Description     : Send data using UDP                                     */
/* Notes           :                                                         */
/*                                                                           */
/*---------------------------------------------------------------------------*/
usys     udp_send (u16 size, void *data, u16 srcPort, u16 dstPort,
                   u32 ipDst, u8 ipTos)
{ 
#if 0
    usys   cpySize, sizeProcessed ;
    usys   offset,  dataSize      ;

    u8     *src    ;
    buf_t  *buf    ;

    /* Calc the data size of the buffer */
    dataSize = net.mtu - fnsSD.netHeaderSize - MAX_IP_HEADER_SIZE ;

    /* Calc the data offset in the buffer */
    offset   = fnsSD.netHeaderSize + MAX_IP_HEADER_SIZE +
               UDP_HEADER_SIZE ;


    /* Start building datagrams of size that matches the MTU */
    sizeProcessed = 0 ;
    src = data ;

    do 
    {
       /* Get a new buffer */
       buf = net_buf_get () ;     
       if (buf == NULL_PTR)
          return E_MEM ;

       /* Copy data into buffer */
       if ((size - sizeProcessed) <= dataSize)
       {
          cpySize = dataSize  ;
       } else {
          cpySize = size - sizeProcessed ;
       }

       memcpy (&buf->data [offset], src, cpySize) ;

       /* Set src port */
       SET_SRC (&buf->data [fnsSD.netHeaderSize + MAX_IP_HEADER_SIZE], 
                srcPort) ;

       /* Set dst port */
       SET_DST (&buf->data [fnsSD.netHeaderSize + MAX_IP_HEADER_SIZE], 
                dstPort) ;

       /* Set length   */
       SET_LEN (&buf->data [fnsSD.netHeaderSize + MAX_IP_HEADER_SIZE], 
                cpySize) ;

       src += cpySize ;
       sizeProcessed += cpySize ; 
       
    } while (sizeProcessed != size) ;
#endif

    return GOOD ;

} /* End of function udp_send () */


/*---------------------------------------------------------------------------*/
/*                                                                           */
/* Function Name   : udp_send                                                */
/* Description     : Send data using UDP                                     */
/* Notes           :                                                         */
/*                                                                           */
/*---------------------------------------------------------------------------*/
usys     udp_init (void)
{
    return GOOD ;
} /* End of function udp_init () */
#endif
