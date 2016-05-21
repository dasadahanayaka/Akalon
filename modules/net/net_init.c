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
/* File Name       : net_init.c                                              */
/* Description     : Network stack entry point and task                      */
/* Notes           :                                                         */
/*                                                                           */
/*---------------------------------------------------------------------------*/
#include <akalon.h>
#include <stdio.h>
#include <string.h>

#include "net_priv.h"


net_inst_t  tmp_inst ;
link_t      net_link ;


u8   buf [128] ;



/*---------------------------------------------------------------------------*/
/*                                                                           */
/* Function Name   : net_task                                                */
/* Description     : Entry Point for the Network Task                        */
/* Notes           :                                                         */
/*                                                                           */
/*---------------------------------------------------------------------------*/
void     net_task  (usys arg0)
{
    eth_hdr_t      *eth_pkt   ;
    net_inst_t     *net_inst  ;
    usys           stat, size ;


    net_inst = (net_inst_t *) arg0 ;

    /* Initialize the ARP Unit */
    if ((stat = arp_init (net_inst)) != GOOD)
    {
       printf("ERR: (NET) arp_init(). Stat = 0x%x\n", stat) ;
       /* <-- DO */
    }


    for (;;)
    {
       if (net_link.le_rx != NULL)
	  if (net_link.le_rx (WAIT_FOREVER, 128, buf, &size) == GOOD)
	  {
	     /* Process frames that's only for us and broadcast frames */
	     if (((buf[0] != 0xff) && (buf[0] != net_inst->mac[0])) ||
                 ((buf[1] != 0xff) && (buf[1] != net_inst->mac[1])) || 
                 ((buf[2] != 0xff) && (buf[2] != net_inst->mac[2])) || 
                 ((buf[3] != 0xff) && (buf[3] != net_inst->mac[3])) || 
                 ((buf[4] != 0xff) && (buf[4] != net_inst->mac[4])) || 
                 ((buf[5] != 0xff) && (buf[5] != net_inst->mac[5])))
	        continue ;

	     eth_pkt = (eth_hdr_t *) buf ;

	     switch (ntohs(eth_pkt->eth_type))
	     {
	        case ETH_TYPE_IPV4:
                     ipv4_pkt_rx (net_inst, buf) ;
		     break ;

	        case ETH_TYPE_ARP:
                     arp_pkt_rx (net_inst, buf) ;
		     break ;

	        default :
                     printf ("Unknown Type 0x%x\n",ntohs(eth_pkt->eth_type)) ;
		     break ;
	     }
	  }
    } /* forever loop */

} /* End of Function net_task () */



/*---------------------------------------------------------------------------*/
/*                                                                           */
/* Function Name   : net_init                                                */
/* Description     : Initializes the Network Stack                           */
/* Notes           :                                                         */
/*                                                                           */
/*---------------------------------------------------------------------------*/
usys     net_init (void)
{
    usys id, stat ;
    net_inst_t *net_inst ;

    net_inst = &tmp_inst ;

    /* Initialize the netif_t structure */
    memset (net_inst, 0, sizeof (net_inst_t)) ;
    net_inst->mac [0] = 0x90 ;
    net_inst->mac [1] = 0xe6 ;
    net_inst->mac [2] = 0xba ;
    net_inst->mac [3] = 0x46 ;
    net_inst->mac [4] = 0x67 ;
    net_inst->mac [5] = 0x89 ;

    net_inst->ip_addr = htonl(0xc0a8010a) ; /* 192.168.1.10 */

    /* Initialize the Network Buffers */
    if ((stat = net_buf_init(net_inst)) != GOOD)
    {
       printf("ERR: (NET) net_buf_init(). Stat %d\n", stat) ;
       goto err ;
    }

    /* Create the Network Task...*/
    stat = task_new (110, 0, 8192, 0,0, net_task, 
                     (usys) net_inst,0,0, 
                     "net_task", &id) ;
    if (stat != GOOD)
    {
       printf("ERR: (NET) Creating net_task. Stat %d\n", stat) ;
       return BAD ;
    } 

    /* Initialize the link */
    memset (&net_link, 0, sizeof (link_t)) ;

    return GOOD ;

 err:
    /* <-- DO */
    return BAD ; 

} /* End of function net_init() */



