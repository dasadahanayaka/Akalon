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
/* File Name       : arp.c                                                   */
/* Description     : ARP Code                                                */
/* Notes           :                                                         */
/*                                                                           */
/*---------------------------------------------------------------------------*/
#include <akalon.h>
#include <string.h>
#include <stdio.h>

#include "net_priv.h"


typedef  struct arp_pkt_t
{
    u16  h_type  ;
    u16  p_type  ;
    u8   h_len   ;
    u8   p_len   ;
    u16  oper    ;

    u8   sha [6] ;
    u32  spa     ;

    u8   tha [6] ;
    u32  tpa     ;

} __attribute__((packed)) arp_pkt_t ;


typedef  struct arp_cache_t
{
    u32  ip_addr  ;
    u8   mac_addr ;
} arp_cache_t ;

static   arp_cache_t    arp_cache [ARP_CACHE_SIZE] ;


void dump_arp_pkt (arp_pkt_t *arp_pkt)
{
    printf ("  htype        =  0x%x\n", ntohs(arp_pkt->h_type)) ; 
    printf ("  ptype        =  0x%x\n", ntohs(arp_pkt->p_type)) ; 
    printf ("  hlen         =  0x%x\n", arp_pkt->h_len) ; 
    printf ("  plen         =  0x%x\n", arp_pkt->p_len) ; 
    printf ("  oper         =  0x%x\n", ntohs(arp_pkt->oper)) ; 
    printf ("  spa          =  0x%x\n", ntohl(arp_pkt->spa))  ; 
    printf ("  tpa          =  0x%x\n", ntohl(arp_pkt->tpa))  ; 
} /* End of function dump_arp_pkt() */


/*---------------------------------------------------------------------------*/
/*                                                                           */
/* Function Name   : update_arp_cache                                        */
/* Description     :                                                         */
/* Notes           :                                                         */
/*                                                                           */
/*---------------------------------------------------------------------------*/
u32      find_arp_listing (u32 ip_addr)
{
    usys i ;

    for (i = 0; i < ARP_CACHE_SIZE; i++)
      if (arp_cache[i].ip_addr == ip_addr)
	 return i ;

    return ARP_CACHE_SIZE ;
} /* End of function find_arp_listing() */



/*---------------------------------------------------------------------------*/
/*                                                                           */
/* Function Name   : update_arp_cache                                        */
/* Description     :                                                         */
/* Notes           :                                                         */
/*                                                                           */
/*---------------------------------------------------------------------------*/
void     update_arp_cache (arp_pkt_t *arp_pkt)
{
    usys arp_listing ;

    /* First see if it's already in the cache */
    arp_listing = find_arp_listing (arp_pkt->spa) ;

} /* End of function update_arp_pkt() */




/*---------------------------------------------------------------------------*/
/*                                                                           */
/* Function Name   : arp_pkt_rx                                              */
/* Description     :                                                         */
/* Notes           :                                                         */
/*                                                                           */
/*---------------------------------------------------------------------------*/
void     arp_pkt_rx (net_inst_t *net_inst, net_buf_t *net_buf)
{
    arp_pkt_t *arp_pkt_in, *arp_pkt_out ;
    pkt_t *pkt ;
    u32   crc, *tmp32 ;

    /* Get to the ARP Portion of the packet */
    arp_pkt_in = (arp_pkt_t *) ( ((usys) net_buf->data) + ETH_HDR_SIZE ) ;

    /* Cache if it's an ARP reply */
    if ((ntohs(arp_pkt_in->oper) == 2) ||   /* Arp Reply pkt  */
        (arp_pkt_in->tpa == arp_pkt_in->spa))  /* Gratuitous pkt */
    {
       update_arp_cache (arp_pkt_in) ;
       printf ("Cache Arp Pkt !!!\n") ;
       return ;
    }

    /* Check if the Arp Request is for us */
    if (net_inst->ip_addr == arp_pkt_in->tpa)
    {
       /* ARP request is for us !!! */

       /* Alloc the frame */
       if ((pkt = pkt_alloc (net_inst)) == NULL)
       {
          printf ("ERR: pkt_alloc() failed in handle_arp_pkt() !!\n") ;
          return ;
       }

       /* Initialize the eth header */
       eth_hdr_init (net_inst, pkt, arp_pkt_in->sha, ETH_TYPE_ARP) ;

       arp_pkt_out = (arp_pkt_t *) (((usys) pkt) + ETH_HDR_SIZE) ;

       arp_pkt_out->h_type = htons((u16) 1) ;
       arp_pkt_out->p_type = htons(ETH_TYPE_IPV4) ;
       arp_pkt_out->h_len  = 6 ;
       arp_pkt_out->p_len  = 4 ;
       arp_pkt_out->oper   = htons(2) ; /* Reply */

       memcpy (arp_pkt_out->sha, net_inst->mac, 6) ;
       arp_pkt_out->spa = net_inst->ip_addr ;

       memcpy (arp_pkt_out->tha, arp_pkt_in->sha, 6) ;
       arp_pkt_out->tpa = arp_pkt_in->spa ;

       /* Calculate the CRC. Clean <-- DO */
       crc = eth_crc_calc ((u8*) pkt, 60) ;
       tmp32  = (u32 *) pkt ;
       tmp32 += 15 ;
       *tmp32 = crc ;

       /* Now send the packet */
       pkt_send (net_inst, pkt, 64) ;

       /* Free the Packet */
       pkt_free (net_inst, pkt) ;
    } 

} /* End of function arp_pkt_rx () */



/*---------------------------------------------------------------------------*/
/*                                                                           */
/* Function Name   : clear_arp_cache                                         */
/* Description     :                                                         */
/* Notes           :                                                         */
/*                                                                           */
/*---------------------------------------------------------------------------*/
void     clear_arp_cache (void)
{
    memset (arp_cache, 0, sizeof (arp_cache_t) * ARP_CACHE_SIZE) ;
} /* End of function clear_arp_cache() */



/*---------------------------------------------------------------------------*/
/*                                                                           */
/* Function Name   : arp_init                                                */
/* Description     :                                                         */
/* Notes           :                                                         */
/*                                                                           */
/*---------------------------------------------------------------------------*/
usys     arp_init (net_inst_t *net_inst)
{
    u8 dst_mac [6] = {0xff,0xff,0xff,0xff,0xff,0xff} ;
    arp_pkt_t *arp_pkt ;
    pkt_t *pkt ;
    u32 crc, *tmp32 ;

    /* Clear the ARP table */
    clear_arp_cache() ;

    /** Send a Gratuitous ARP **/

    /* Alloc the frame */
    if ((pkt = pkt_alloc (net_inst)) == NULL)
    {
       printf ("ERR: pkt_alloc() failed in arp_init() !!\n") ;
       return BAD ;
    }

    /* Initialize the eth header */
    eth_hdr_init (net_inst, pkt, dst_mac, ETH_TYPE_ARP) ;

    /* Fill out the ARP data according to the ARP Request Method */
    /* which sets the TPA=SPA and the THA = 0                    */
    arp_pkt = (arp_pkt_t *) (((usys) pkt) + ETH_HDR_SIZE) ;

    arp_pkt->h_type = htons((u16) 1) ;
    arp_pkt->p_type = htons(ETH_TYPE_IPV4) ;
    arp_pkt->h_len  = 6 ;
    arp_pkt->p_len  = 4 ;

    arp_pkt->oper   = htons(1) ;

    memcpy (arp_pkt->sha, net_inst->mac, 6) ;
    arp_pkt->spa = net_inst->ip_addr ;

    memset (arp_pkt->tha, 0, 6) ;
    arp_pkt->tpa = net_inst->ip_addr ;

    /* Calculate the CRC */
    crc = eth_crc_calc ((u8*) pkt, 60) ;
    tmp32  = (u32 *) pkt ;
    tmp32 += 15 ;
    *tmp32 = crc ;

    /* Now send the packet */
    pkt_send (net_inst, pkt, 64) ;

    /* Free the Packet */
    pkt_free (net_inst, pkt) ;

    return GOOD ;

} /* End of function arp_init () */





