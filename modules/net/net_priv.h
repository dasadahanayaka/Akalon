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
/* File Name       : net_priv.h                                              */
/* Description     : Network Stack's internal interface file                 */
/* Notes           :                                                         */
/*                                                                           */
/*---------------------------------------------------------------------------*/
#ifndef  NET_PRIV_H
#define  NET_PRIV_H


/*------------------*/
/* Buffer Structure */
/*------------------*/
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


/*------------*/
/* Prototypes */
/*------------*/

extern   usys      net_buf_init   (void) ;
extern   buf_t     *net_buf_get   (void) ;
extern   usys      net_buf_free   (buf_t *buf) ;

extern   usys      ether_init     (void) ;
extern   usys      ipv4_init      (void) ;
extern   usys      tcp_init       (void) ;
extern   usys      udp_init       (void) ;






#if 0

#define  U16SWAP(v) ((((u16) v) << 8) | (((u16) v) >> 8))

#define  U32SWAP(v) (( ((u32) v)               << 24) |   \
                     ((((u32) v) & 0x0000ff00) << 16) |   \
                     ((((u32) v) & 0x00ff0000) >> 16) |   \
                     ( ((u32) v)               >> 24))

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

/*------------------*/
/* Protocol Headers */
/*------------------*/
typedef  struct    tcp_t 
{
} tcp_t ;

typedef  struct    udp_t
{
} udp_t ;

typedef  struct    ip_t
{
} ip_t ;

typedef  struct    ether_t 
{
} ether_t ;

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
