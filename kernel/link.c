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
/* File Name       : link.c                                                  */
/* Description     : Akalon Link Stacker functions                           */
/* Notes           : Probably the hardest code in Akalon to understand !!!   */
/*                                                                           */
/*---------------------------------------------------------------------------*/
#include <akalon.h>


/*---------------------------------------------------------------------------*/
/*                                                                           */
/* Function Name   : os_link                                                 */
/* Description     : Connect a Tx and a Rx Module/Device to an Upper Module  */
/* Notes           :                                                         */
/*                                                                           */
/*---------------------------------------------------------------------------*/
usys     os_link   (link_t *top, link_t *rx, link_t *tx)
{
    if (top == NULL)
       return BAD ;

    /** RX **/
    if (rx != NULL)
    {
       /* Connect the Receiver Module's Tx function    */
       /* variable to the Top Module's Rx function.    */

       rx->tx_func = top->rx_func ;

       /* Connect the Top Module's external Rx config  */
       /* variable to the Rx Module's config function. */

       top->rx_conf = rx->conf_func ; 
    }

    /** TX **/
    if (tx != NULL)
    {
       /* Connect the Top Module's Tx function variable */
       /* to the Transmit Module's Rx function          */

       top->tx_func = tx->rx_func ; 

       /* Connect the Top Module's external Tx config   */
       /* variable to the Tx Module's config function.  */

       top->tx_conf = tx->conf_func ;
    }

    return GOOD ;

} /* End of function os_link() */


