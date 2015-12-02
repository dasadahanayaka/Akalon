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
/* File Name       : cli.h                                                   */
/* Description     : Akalon's Command Line Interface                         */
/* Notes           :                                                         */
/*                                                                           */
/*---------------------------------------------------------------------------*/
#ifndef  CLI_H
#define  CLI_H

typedef  struct    func_t     
{   /* Make sure this structure is sync'd with /tools/gen_symbols */
    void (*func)() ;
    char *name ;
} func_t ;

typedef  struct    var_t     
{   /* Make sure this structure is sync'd with /tools/gen_symbols */
    void *addr ;
    char *name ;
} var_t ;

extern   func_t    sym_funcs[]   ;
extern   var_t     sym_vars[]    ;

#endif   /* ! CLI_H */
