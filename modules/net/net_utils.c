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
/* File Name       : net_utils.c                                             */
/* Description     : Network stack utilities                                 */
/* Notes           :                                                         */
/*                                                                           */
/*---------------------------------------------------------------------------*/
#include <akalon.h>
#include "net_priv.h"


/*---------------------------------------------------------------------------*/
/*                                                                           */
/* Function Name   : crc16_calc                                              */
/* Description     : Calculate the 16-bit CRC the Driver                     */
/* Notes           :                                                         */
/*                                                                           */
/*---------------------------------------------------------------------------*/
u16      crc16_calc (void *data, u16 len)
{
    u32  val32 = 0 ;
    u16  tmp16 ;
    u8   *tmp8 ;
    usys i ;

    tmp8 = (u8 *) data ;

    for (i = 0; i < len; i += 2)
    {
       tmp16  = (*tmp8++) << 8 ;
       tmp16 |= *tmp8++ ;

       val32 += tmp16 ;
    }

    if (i != len)
       val32 += *tmp8 ;

    /* Add the residue */
    val32 = (val32 >> 16) + (val32 & 0xffff) ;

    /* Check if the residue created more residue */
    if (val32 & 0xffff0000)
       val32 = (val32 >> 16) + (val32 & 0xffff) ;

    return htons(val32) ;

} /* End of function crc16_calc() */



/*---------------------------------------------------------------------------*/
/*                                                                           */
/* Function Name   : pkt_send                                                */
/* Description     : Send a Network Packet to the Driver                     */
/* Notes           :                                                         */
/*                                                                           */
/*---------------------------------------------------------------------------*/
usys     pkt_send (net_inst_t *net_inst, pkt_t *pkt, usys size) 
{
    if (net_link.tx_func != NULL)
       if (net_link.tx_func (DONT_WAIT, size, pkt) == GOOD)
	  return GOOD ;

    return BAD ;
} /* End of function pkt_send() */



/*---------------------------------------------------------------------------*/
/*                                                                           */
/* Function Name   : dev_buf_free                                            */
/* Description     : Release the device buffer                               */
/* Notes           :                                                         */
/*                                                                           */
/*---------------------------------------------------------------------------*/
void     dev_buf_free (net_inst_t *inst, net_buf_t *buf) 
{
    if (inst->buf_free_func == NULL)
       return ;

    inst->buf_free_func (inst->buf_free_func_arg, buf) ;

} /* End of function dev_buf_free() */
