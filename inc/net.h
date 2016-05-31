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
/* File Name       : net.h                                                   */
/* Description     : Akalon's Network Interface                              */
/* Notes           :                                                         */
/*                                                                           */
/*---------------------------------------------------------------------------*/
#ifndef  NET_H
#define  NET_H

typedef  struct    net_buf_t
{
    usys                size  ;
    void                *data ;
    struct net_buf_t    *next ;
} net_buf_t ;

#define  CMD_BUF_FREE_FUNC_GET    1

#endif   /* ! NET_H */
