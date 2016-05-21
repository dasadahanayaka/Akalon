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

#define  ETH_TYPE_IPV4            0x0800
#define  ETH_TYPE_ARP             0x0806

#define  IPV4_PROTOCOL_ICMP       1

typedef  struct    eth_hdr_t
{
    u8   dst_mac   [6] ;
    u8   src_mac   [6] ;

    u16  eth_type  ;
} __attribute__((packed)) eth_hdr_t ;

#define  ETH_HDR_SIZE        14


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

#define  IPV4_HDR_SIZE  20

#define  IPV4_TYPE_ICMP 1

/*** Internal Interfaces ***/ 

typedef  struct    net_inst_t
{
    u8   mac [6]   ;
    u32  ip_addr   ;
} net_inst_t ;

typedef  void      *pkt_t ;


extern   usys      net_buf_init   (net_inst_t *net_inst) ;
extern   usys      arp_init       (net_inst_t *net_inst) ;


extern   pkt_t     *pkt_alloc     (net_inst_t *net_inst) ;
extern   void      pkt_free       (net_inst_t *net_inst, pkt_t *pkt) ;
extern   usys      pkt_send       (net_inst_t *net_inst, pkt_t *pkt,
                                   usys size) ;

extern   void      arp_pkt_rx     (net_inst_t *netif, void *buf) ;
extern   void      ipv4_pkt_rx    (net_inst_t *netif, void *buf) ;
extern   void      icmp_pkt_rx    (net_inst_t *netif, void *buf) ;

extern   usys      eth_hdr_init   (net_inst_t *net_inst, pkt_t *pkt,
                                   u8 *dst_mac, u16 eth_type) ;

extern   void      ipv4_hdr_init  (net_inst_t *net_inst, 
                                   ipv4_hdr_t *ipv4_pkt,
                                   usys len,  usys protocol, u32 dst) ;


extern   u32       eth_crc_calc   (void *pkt, usys size) ;
extern   u16       crc16_calc     (void *buf, u16 size) ;



/* Net module configuration */
#define  ARP_CACHE_SIZE      64







#if 0


typedef  struct    buf_t 
{
    u8      *data  ;

    struct  buf_t  *p_buf  ;
    struct  buf_t  *n_buf  ;
} buf_t  ;



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

extern   net_t     net ;



extern   usys      ipv4_init      (void) ;
extern   usys      tcp_init       (void) ;
extern   usys      udp_init       (void) ;


/*------------------*/
/* Protocol Headers */
/*------------------*/
typedef  struct    tcp_t 
{
} tcp_t ;

typedef  struct    udp_t
{
} udp_t ;



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
