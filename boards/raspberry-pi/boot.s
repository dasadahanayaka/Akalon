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
/* File Name       : boot.s                                                  */
/* Description     : First code to run on the R-PI                           */
/* Notes           :                                                         */
/*                                                                           */
/*---------------------------------------------------------------------------*/	
	.text

.global _start
.global _int_vec
	
_start:
	ldr	pc, _reset_vec
	ldr	pc, _undef_vec
	ldr	pc, _sw_vec
	ldr	pc, _pf_vec
	ldr	pc, _da_vec
	ldr	pc, _un_vec
	ldr	pc, _int_vec
	ldr	pc, _fiq_vec

_reset_vec:	.word	_reset
_undef_vec:	.word	_dummy
_sw_vec:	.word	_dummy	
_pf_vec:	.word	_dummy
_da_vec:	.word	_dummy
_un_vec:	.word	_dummy
_int_vec:	.word	irq_isr
_fiq_vec:	.word	_dummy

_reset:
	mov r0, #0x8000
	mov r1, #0x0000
	ldmia r0!,{r2, r3, r4, r5, r6, r7, r8, r9}
	stmia r1!,{r2, r3, r4, r5, r6, r7, r8, r9}
	ldmia r0!,{r2, r3, r4, r5, r6, r7, r8, r9}
	stmia r1!,{r2, r3, r4, r5, r6, r7, r8, r9}

	mov	sp, #(64 * 1024 * 1024)
	bl	bsp_pre_init

_dummy:
	b	_dummy
