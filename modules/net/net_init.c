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
/* Notes           : Only 1 instance of the network stack is allowed         */
/*                                                                           */
/*---------------------------------------------------------------------------*/
#include <akalon.h>
#include <stdio.h>
#include <string.h>

#include "net_priv.h"

#define  MAX_MSGS   256

static   net_inst_t  net_inst ;
link_t   net_link ;



/*---------------------------------------------------------------------------*/
/*                                                                           */
/* Function Name   : rx_func                                                 */
/* Description     : Net Module's Receiver Function                          */
/* Notes           : Driver calls this function to provide packets           */
/*                                                                           */
/*---------------------------------------------------------------------------*/
static   usys      rx_func (usys arg, usys size, void *data)
{
    /* Send a message to the net_task */

    return (msg_send (net_inst.task_id, sizeof (usys), &data)) ;
} /* End of function rx_func() */



/*---------------------------------------------------------------------------*/
/*                                                                           */
/* Function Name   : net_task                                                */
/* Description     : Entry Point for the Network Task                        */
/* Notes           :                                                         */
/*                                                                           */
/*---------------------------------------------------------------------------*/
void     net_task  (usys arg0)
{
    net_inst_t *net_inst ;

    net_buf_t  *net_buf  ;
    eth_hdr_t  *eth_pkt  ;

    usys id, stat, size  ;
    u8   *buf ;

    /* Get the instance...*/
    net_inst = (net_inst_t *) arg0 ;

    /* Initialize the ARP Unit */
    if ((stat = arp_init (net_inst)) != GOOD)
    {
       printf("ERR: (NET) arp_init(). Stat = 0x%x\n", stat) ;
       /* <-- DO */
    }

    /* Call the bottom module for configuration */
    if (net_link.rx_conf_var != NULL)
        net_link.rx_conf_var (CMD_BUF_FREE_FUNC_GET, 
                              &net_inst->buf_free_func,
                              &net_inst->buf_free_func_arg) ;

    /* Loop here forever */
    for (;;)
    {
       /* Get a message...Currently only from the driver */
       if (msg_get (&id, &size, &net_buf, WAIT_FOREVER) != GOOD)
          continue ;

       buf = net_buf->data ;

       /* Process frames that's only for us and broadcast frames */
       if (((buf[0] != 0xff) && (buf[0] != net_inst->mac[0])) ||
           ((buf[1] != 0xff) && (buf[1] != net_inst->mac[1])) || 
           ((buf[2] != 0xff) && (buf[2] != net_inst->mac[2])) || 
           ((buf[3] != 0xff) && (buf[3] != net_inst->mac[3])) || 
           ((buf[4] != 0xff) && (buf[4] != net_inst->mac[4])) || 
           ((buf[5] != 0xff) && (buf[5] != net_inst->mac[5])))
	  goto free_buf ;

       /* Packet is for us...Process it */
       eth_pkt = (eth_hdr_t *) net_buf->data ;

       switch (ntohs(eth_pkt->eth_type))
       {
          case ETH_TYPE_IPV4:
               ipv4_pkt_rx (net_inst, net_buf) ;
               break ;

	  case ETH_TYPE_ARP:
               arp_pkt_rx (net_inst, net_buf) ;
	       break ;

	  default :
               printf ("Unknown Type 0x%x\n",ntohs(eth_pkt->eth_type)) ;
	       break ;
       }

free_buf:
       dev_buf_free (net_inst, net_buf) ;

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
    usys stat ;

    /* Initialize the net_inst struct */
    memset (&net_inst, 0, sizeof (net_inst_t)) ;
    net_inst.mac [0] = 0x90 ;
    net_inst.mac [1] = 0xe6 ;
    net_inst.mac [2] = 0xba ;
    net_inst.mac [3] = 0x46 ;
    net_inst.mac [4] = 0x67 ;
    net_inst.mac [5] = 0x89 ;

    net_inst.ip_addr = htonl(0xc0a8010a) ; /* 192.168.1.10 */

    /* Initialize the Network Buffers */
    if ((stat = net_buf_init(&net_inst)) != GOOD)
    {
       printf("ERR: (NET) net_buf_init(). Stat %d\n", stat) ;
       goto err ;
    }

    /* Create the Network Task...*/
    stat = task_new (110, 0, 8192, MAX_MSGS, sizeof (usys), net_task, 
                     (usys) &net_inst,0,0, 
                     "net_task", &net_inst.task_id) ;
    if (stat != GOOD)
    {
       printf("ERR: (NET) Creating net_task. Stat %d\n", stat) ;
       return BAD ;
    } 

    /* Initialize the link */
    memset (&net_link, 0, sizeof (link_t)) ;
    net_link.rx_func = rx_func ;

    return GOOD ;

 err:
    /* <-- DO */
    return BAD ; 

} /* End of function net_init() */



