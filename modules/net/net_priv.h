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
/* File Name       : net_priv.h                                              */
/* Description     : Network Stack's internal interface file                 */
/* Notes           :                                                         */
/*                                                                           */
/*---------------------------------------------------------------------------*/
#ifndef  NET_PRIV_H
#define  NET_PRIV_H

#define  NET_MOD_DEBUG


#define  U16SWAP(v) ( ((v & 0x00ff) << 8) | ((v & 0xff00) >> 8) )

#define  U32SWAP(v) ( ((v & 0x000000ff) << 24) | \
                      ((v & 0x0000ff00) <<  8) | \
                      ((v & 0x00ff0000) >>  8) | \
                      ((v & 0xff000000) >> 24) )

#if defined (LITTLE_ENDIAN)
#define  htons(v)       U16SWAP(v)
#define  ntohs(v)       U16SWAP(v)
#define  htonl(v)       U32SWAP(v)
#define  ntohl(v)       U32SWAP(v)
#else
#define  htons(v)       v
#define  ntohs(v)       v
#define  htonl(v)       v
#define  ntohl(v)       v
#endif

/************/
/* Ethernet */
/************/

typedef  struct    eth_hdr_t
{
    u8   dst_mac   [6] ;
    u8   src_mac   [6] ;

    u16  eth_type  ;
} __attribute__((packed)) eth_hdr_t ;

#define  ETH_HDR_SIZE        14

#define  ETH_TYPE_IPV4       0x0800
#define  ETH_TYPE_ARP        0x0806


/********/
/* IPV4 */
/********/

typedef  struct    ipv4_hdr_t
{
    u8   ver_ihl        ;
    u8   diff           ;  /* Was TOS */
    u16  len            ;
    u16  id             ;
    u16  flags_offset   ;
    u8   ttl            ;
    u8   protocol       ;
    u16  csum           ;
    u32  src            ;
    u32  dst            ;
} __attribute__((packed)) ipv4_hdr_t ;

#define  IPV4_HDR_SIZE       20

#define  IPV4_PROTOCOL_ICMP  0x01
#define  IPV4_PROTOCOL_UDP   0x11

/*******/
/* UDP */
/*******/

typedef  struct    udp_hdr_t
{
    u16  src_port  ;
    u16  dst_port  ;
    u16  len       ;
    u16  csum      ;
} __attribute__((packed)) udp_hdr_t ;

#define  UDP_HDR_SIZE        8




/*****************/
/* Network Stack */
/*****************/

/* Net module configuration */
#define  ARP_CACHE_SIZE      64
#define  MAX_NETIFS          4


typedef  struct    netif_t
{
    u8   mac [6]   ;

    u32  ip_addr   ;
    u16  ipv4_id   ;

    void (*buf_free_func) (usys, net_buf_t *) ;
    usys buf_free_func_arg ; 

    usys task_id   ; /* Doesn't belong here */

    void *ipv4_mgr ;

} netif_t ;


typedef  struct    net_t
{
    usys       num_netif ;
    netif_t    *netif[MAX_NETIFS] ;
} net_t ;



typedef  void      *pkt_t ;


extern   usys      net_buf_init   (netif_t *netif) ;
extern   usys      ipv4_init      (netif_t *netif) ;
extern   usys      arp_init       (netif_t *netif) ;


extern   pkt_t     *pkt_alloc     (netif_t *netif) ;
extern   void      pkt_free       (netif_t *netif, pkt_t *pkt) ;
extern   usys      pkt_send       (netif_t *netif, pkt_t *pkt,
                                   usys size) ;

extern   void      arp_pkt_rx     (netif_t *netif, net_buf_t *buf) ;
extern   void      ipv4_pkt_rx    (netif_t *netif, net_buf_t *buf) ;
extern   void      icmp_pkt_rx    (netif_t *netif, net_buf_t *buf) ;
extern   void      udp_pkt_rx     (netif_t *netif, net_buf_t *buf) ;

extern   usys      eth_hdr_init   (netif_t *netif, pkt_t *pkt,
                                   u8 *dst_mac, u16 eth_type) ;

extern   void      ipv4_hdr_init  (netif_t *netif, ipv4_hdr_t *ipv4_pkt,
                                   usys len,  usys protocol, u32 dst) ;

extern   u32       eth_crc_calc   (void *pkt, usys size) ;
extern   u16       crc16_calc     (void *buf, u16 size) ;
extern   void      dev_buf_free   (netif_t *netif, net_buf_t *buf) ;


#if defined(NET_MOD_DEBUG)
#include <stdio.h>
extern   void      net_buf_dump   (net_buf_t  *net_buf) ;
extern   void      eth_pkt_dump   (eth_hdr_t  *pkt) ;
extern   void      ipv4_pkt_dump  (ipv4_hdr_t *pkt) ;
extern   void      upd_pkt_dump   (udp_hdr_t *pkt)  ;

#define  NET_DEBUG(a...)     printf(a)
#define  NET_BUF_DUMP(a)     net_buf_dump(a)
#define  ETH_PKT_DUMP(a)     eth_pkt_dump(a)
#define  IPV4_PKT_DUMP(a)    ipv4_pkt_dump(a)
#define  UDP_PKT_DUMP(a)     udp_pkt_dump(a)
#else
#define  NET_DEBUG(a...)
#define  NET_BUF_DUMP(a)
#define  ETH_PKT_DUMP(a)
#define  IPV4_PKT_DUMP(a)
#define  UDP_PKT_DUMP(a)
#endif










#if 0
/*------------------------*/
/* Network Info Structure */
/*------------------------*/
typedef  struct    net_t 
{
    buf_t     *bufs      ;
    buf_t     *free_bufs ;

    usys      mtu  ;

    void      *udp ;
    void      *tcp ;
} net_t  ;

extern   usys      ipv4_init      (void) ;
extern   usys      tcp_init       (void) ;

/*------------------*/
/* Protocol Headers */
/*------------------*/
typedef  struct    tcp_t 
{
} tcp_t ;

/*---------------------*/
/* Protocol Structure  */
/*---------------------*/
typedef  struct    protoST 
{
    /* IP Info */
    u8      tos          ;
    u8      proto        ;

    u16     srcPort      ;
    u16     dstPort      ;

    u16     len          ;
} protoST  ;

/*---------------------*/
/* FNS Core Structure  */
/*---------------------*/

typedef  struct    fnsST          
{
    usys    netHeaderSize   ;
} fnsST  ;
#endif


#endif  /* ! NET_PRIV_H */
