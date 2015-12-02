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
/* File Name       : link.c                                                  */
/* Description     : Akalon Link Stacker functions                           */
/* Notes           :                                                         */
/*                                                                           */
/*---------------------------------------------------------------------------*/
#include <akalon.h>
#include <stdio.h>

#include "kernel.h"

/*---------------------------------------------------------------------------*/
/*                                                                           */
/* Function Name   : os_link                                                 */
/* Description     : Connect a Tx and a Rx Module/Device to an Upper Module  */
/* Notes           :                                                         */
/*                                                                           */
/*---------------------------------------------------------------------------*/
usys     os_link (link_t *uApp, link_t *lAppRx, link_t *lAppTx)
{
    if (uApp == NULL)
       return BAD ;

    /** RX **/
    if (lAppRx != NULL)
    {
       /* Connect the Upper App's Lower External Rx function */
       /* to the Lower App's Upper Internal Tx Interfaces.   */

       if (lAppRx->ui_tx != NULL)
          uApp->le_rx = lAppRx->ui_tx ;

       /* Connect the Lower App's Upper External Tx function */
       /* to the Upper App's Lower Internal Rx function      */
 
       if (uApp->li_rx != NULL)
          lAppRx->ue_tx = uApp->li_rx ;
    }


    /** TX **/
    if (lAppTx != NULL) 
    {
       /* Connect the Upper App's Lower External Tx function */
       /* to the Lower App's Upper Internal Rx Interfaces.   */

       if (lAppTx->ui_rx != NULL)
          uApp->le_tx = lAppTx->ui_rx ;

       /* Connect the Lower App's Upper External Rx function */
       /* to the Upper App's Lower Internal Tx function      */

       if (uApp->li_tx != NULL)
          lAppTx->ue_rx = uApp->li_tx ;
    }

    return GOOD ;

} /* End of function os_link() */


