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
/* File Name       : asm.s                                                   */
/* Description     : ARM assembly language code                              */
/*                                                                           */	
/* Notes           :                                                         */
/*                                                                           */
/* Calling a C function from ARM assembler code..                            */
/*                                                                           */
/*        r3 - 4th Parameter                                                 */
/*        r2 - 3rd Parameter                                                 */
/*        r1 - 2nd Parameter                                                 */
/*        r0 - 1st Parameter                                                 */
/*        Return Addr                                                        */
/*                                                                           */
/* The return is in r0                                                       */
/*                                                                           */
/* - The EABI for the ARM requires a stack grows downwards full.             */
/* - There is no push or pop instructions in ARM. Every thing has to be done */
/* - manually.                                                               */
/*                                                                           */
/*---------------------------------------------------------------------------*/	
#define ASM        1
.extern irq_stack
	
        .text

/*---------------------------------------------------------------------------*/
/*                                                                           */
/* Function Name   : irq_isr                                                 */
/* Description     : Regular (IRQ) Interrupt Vector                          */
/*                                                                           */	
/* Notes           : When the processor detects a regular interrupt, it      */
/*                   enters the IRQ mode. In this mode, the r13 register is  */
/*                   replaced with another r13 which points to a diffrent    */
/*                   stack which was initialized in cpu_int_init(). This     */
/*                   stack is 2x32-bit words wide and is expected to only    */
/*                   contain the interrupted execution address and the       */
/*                   interrupted cpsr which during an interrupt is copied to */
/*                   spsr.                                                   */
/*                                                                           */	
/*---------------------------------------------------------------------------*/	
.globl  irq_isr
irq_isr:
	/* ISR entry point. A new stack (r13) has been given with  */
	/* a new cpsr and obviously a new r15 (PC). The old stack  */
        /* (r13) is in the SRV mode while the old interrupted PC   */
	/* has been copied to r14 and the old cpsr has been copied */
	/* into spsr. Copy the old PC and the cpsr onto the irq    */
	/* stack.                                                  */
	
	/* Because of the executing pipeline, the value in r14 has */
	/* to subtracted by 0x4 becuase it in the pipeline and was */
	/* just about to get executed.                             */

	sub	r14, #0x4  
	str	r14, [r13]  /* Push interrupted PC */

	/* Read and Save cpsr */	
	mrs	r14, spsr         /* Read */
	str	r14, [r13, #0x4]  /* Save */

	/* The irq_stack now looks like...                         */
	/*                                                         */
	/*  irq_stack [1]   <-- Interrupted cpsr                   */	
	/*  irq_stack [0]   <-- Interrupted r15 (PC)               */
	/*                                                         */	

        /* Move back to the SVC mode becuase irq_run expects to be */
	/* called from the interrupted task's stack.               */
	
        /* Go into SVC mode.                                       */
	msr	cpsr_c, #0xd3
	
	/* In SVC Mode */

	/* Start saving the general registers.                     */
	stmfd	r13, {r0-r15}

	/* The stack now looks like...                             */
	/*                                                         */	
	/*   XXX    <-- r13 (original stack location)              */
	/*   r15    <-- PC                                         */
	/*   r14    <-- LR                                         */
	/*   r13    <-- Stack Pointer (points to XXX)              */	
	/*   r12                                                   */
	/*   r11                                                   */
	/*   r10                                                   */
	/*   r9                                                    */
	/*   r8                                                    */
	/*   r7                                                    */
	/*   r6                                                    */
	/*   r5                                                    */
	/*   r4                                                    */
	/*   r3                                                    */
	/*   r2                                                    */
	/*   r1                                                    */
	/*   r0                                                    */
	/*                                                         */
	/* However, r15 is not correct. The correct value is in    */
        /* irq_stack. We grab it from there and save it to 4 bytes */
	/* below the stack pointer.                                */
	
	ldr	r0, =irq_stack      /* Get Location */

	/* Save PC */
	ldr	r1, [r0]            /* Load         */ 
	str	r1, [r13, #-0x4]    /* Save it.     */

	/* Save cpsr */
	ldr	r1, [r0, #0x4]      /* Load         */
	str	r1, [r13, #-0x44]!  /* Save it.     */

	/* The previous instruction also updated the stack pointer */
	/* so when int_run() is called, the stack looks like..     */
	/*   XXX    <-- r13 (original stack location)              */
	/*   r15    <-- PC                                         */
	/*   r14    <-- LR                                         */
	/*   r13    <-- Stack Pointer (points to XXX)              */	
	/*   r12                                                   */
	/*   r11                                                   */
	/*   r10                                                   */
	/*   r9                                                    */
	/*   r8                                                    */
	/*   r7                                                    */
	/*   r6                                                    */
	/*   r5                                                    */
	/*   r4                                                    */
	/*   r3                                                    */
	/*   r2                                                    */
	/*   r1                                                    */
	/*   r0                                                    */
	/*   cpsr   <-- r13 (stack pointer)                        */
	
	/* Call int_run() which will execute the user ISR and if   */
	/* a higher priority task became active, it will return    */
	/* the higher priority task's context (i.e it's stack)     */
	/* in r0 so this function can execute it.                  */
	
	mov	r0, #0x0   /* IRQ is given a interrupt number of 0 */
	bl	int_run    

	/* Check if a higher priority task became active as a      */
	/* result of the interrupt (int_run returns that info in   */
	/* r0. If it's zero, no higher task became active.         */
	
	cmp	r0, #0x0
	beq     1f       /* No higher task became active. Load the */
	                 /* existing context by branching to 1f    */

	/* If branch 1f was not taken, this implies a higher task  */
	/* did become active. This higher task's context is in its */
	/* stack whose location was returned by int_run in r0. But */
	/* before switching to the new stack (i.e context), the    */
	/* old stack location needs to be saved to the TCB by      */
	/* calling save_task_state(). We save r0, becuase it can   */
	/* be trashed in the save_task_state() call. r0 is picked  */
	/* it cannot be trashed in a function call.                */
	
	mov	r6, r0
	mov	r0, r13         
	bl	save_task_state    /* Save context */
	mov	r13, r6

1:
	/* Load the exisiting context or the new context. The code */
	/* was made the same by just changing the stack.           */
	
	/* Read and Load the spsr. When the "^" operator is used,  */
	/* the spsr is automatically loaded into the cpsr.         */ 

	ldr	r0, [r13]      /* Read */
	add	r13, #0x4      /* Point to r0 */
	msr	spsr, r0       /* Load */

	/* Now increment the stack by 4 bytes to point to r0 which */
	/* is 4 bytes above cpsr.                                  */

	/* Update all the general registers, including the PC      */
	/* which is r15 and load the spsr into the cpsr. Noop's    */
	/* needed to clear the pipeline.                           */
	
	ldmfd	r13, {r0-r15}^ 
	nop
	nop
	nop

	

/*---------------------------------------------------------------------------*/
/*                                                                           */
/* Function Name   : irq_isr                                                 */
/* Description     : Regular (IRQ) Interrupt Vector                          */
/* Notes           : Clean up this function !!!                              */
/*                                                                           */
/*---------------------------------------------------------------------------*/
.globl  irq_vector
irq_vector:
	ldr	pc, [pc, #-0xf20] 
	nop
	nop
	nop

	
/*---------------------------------------------------------------------------*/
/*                                                                           */
/* Function Name   : fiq_isr                                                 */
/* Description     : Fast (FIQ) Interrupt Vector                             */
/* Notes           :                                                         */
/*                                                                           */
/*---------------------------------------------------------------------------*/	
.globl  fiq_vector
fiq_vector:	
	/* Should an error be thrown here ?? <-- DO */



/*---------------------------------------------------------------------------*/
/*                                                                           */
/* Function Name   : cpu_task_first                                          */
/* Description     : Spawns the first task which is the idle task.           */
/*                                                                           */
/* Notes           : The stack pointer of the idle task is provided in r0    */
/*                   and this location contains the context of the idle task */
/*---------------------------------------------------------------------------*/
.globl  cpu_task_first
cpu_task_first:
	/*---------------------------------------------------------*/	
	/* Register r0 points to the idle task's stack which looks */
	/* like the following...                                   */
	/*                                                         */
	/*   0xb16b00da   <-- Top of stack                         */
	/*   r15          <-- PC (address of task_start)           */
	/*   r14          <-- LR (address of task_start)           */
	/*   r13          <-- Stack Pointer (points to 0xb16b00da) */	
	/*   r12                                                   */
	/*   r11                                                   */
	/*   r10                                                   */
	/*   r9                                                    */
	/*   r8                                                    */
	/*   r7                                                    */
	/*   r6                                                    */
	/*   r5                                                    */
	/*   r4                                                    */
	/*   r3           <-- 4th Param (arg2)                     */
	/*   r2           <-- 3rd Param (arg1)                     */
	/*   r1           <-- 2nd Param (arg0)                     */
	/*   r0           <-- 1st Param (task entry point)         */		
        /*   cpsr         <-- stack_ptr (sent on r0)               */
	/*                                                         */	
	/*---------------------------------------------------------*/
	
	/* Load cpsr */
	ldr	r1, [r0]
	msr	cpsr, r1

	/* Load the rest of registers */
	add	r0, r0, #0x4
	ldmfd	r0, {r0-r15}

	/* No-ops are needed to clear the pipeline */
	nop   
	nop
	nop



/*---------------------------------------------------------------------------*/
/*                                                                           */
/* Function Name   : cpu_task_switch                                         */
/* Description     : Context switch code                                     */
/*                                                                           */
/* Notes           : Saves the currently running task and switches to the    */
/*                   new task.                                               */
/*                                                                           */
/*                   The syntax for this call is...                          */
/*                                                                           */
/*                      cpu_task_switch (&current_stack, new_stack) ;        */
/*                                                                           */
/*                   At entry...                                             */
/*                                                                           */
/*                      r0 - Location to save the current task's stack       */
/*                           pointer                                         */	
/*                      r1 - The new stack's stack ptr                       */
/*                                                                           */	
/*---------------------------------------------------------------------------*/
.globl  cpu_task_switch
cpu_task_switch :
	
	/* When the current task calls this function, the stack    */
	/* is pointing some valid stack data. Start by saving all  */
	/* registers including r15 (the PC).                       */
	
	stmfd	r13, {r0-r15}

	/* All the general registers are now saved. The stack now  */
	/* looks like this...                                      */
	/*                                                         */	
	/*   XXX    <-- r13 (original stack location)              */
	/*   r15    <-- PC                                         */
	/*   r14    <-- LR                                         */
	/*   r13    <-- Stack Pointer (points to XXX)              */	
	/*   r12                                                   */
	/*   r11                                                   */
	/*   r10                                                   */
	/*   r9                                                    */
	/*   r8                                                    */
	/*   r7                                                    */
	/*   r6                                                    */
	/*   r5                                                    */
	/*   r4                                                    */
	/*   r3                                                    */
	/*   r2                                                    */
	/*   r1                                                    */
	/*   r0                                                    */		
	/*                                                         */
	/* Although r15 (the PC) was saved, we never go back to    */
	/* this location when this task is re-run. If that was     */
	/* done, then when this task is re-run, it will start      */
	/* executing from the middle of this function. Instead,    */
	/* we copy r14 (which is where this function was called)   */
	/* to r15. Now when this task is re-run, it will return    */
	/* back to the caller.                                     */
	
	/* Copy r14 -> r15 */ 
	str	r14, [r13, #-0x4] 
	
        /* Now save the CPSR. But before doing this, the stack ptr */
	/* needs to be moved since it was not moved after saving   */
	/* all the registers. This is done by simply moving r13    */
	/* (SP) by 68 (0x44) bytes below the current location.     */
	
	sub	r13, #0x44 

	/* Grab and Save CPSR */
	mrs	r2, cpsr  
	str	r2, [r13]

	/* The current task's context (i.e it's registers) are now */
	/* saved. Thus, it's stack now looks like...               */
	/*                                                         */	
	/*   XXX    <-- r13 (original stack location)              */
	/*   r15    <-- PC                                         */
	/*   r14    <-- LR                                         */
	/*   r13    <-- Stack Pointer (points to XXX)              */	
	/*   r12                                                   */
	/*   r11                                                   */
	/*   r10                                                   */
	/*   r9                                                    */
	/*   r8                                                    */
	/*   r7                                                    */
	/*   r6                                                    */
	/*   r5                                                    */
	/*   r4                                                    */
	/*   r3                                                    */
	/*   r2                                                    */
	/*   r1                                                    */
	/*   r0                                                    */		
	/*   cpsr   <-- Current location of stack (r13)            */
	/*                                                         */
	/* The only other thing left to do is to save the current  */
	/* stack location to the task's TCB (i.e stack_ptr). This  */
	/* location is provided by r0.                             */
	/*                                                         */		

	str	r13, [r0] 

	/***** The current stack is now completely saved !!!  ******/

	/* Now load the new task's context which is located on its */
	/* stack. This location of this stack is pointed by r1 and */
	/* its "structure" is similar to what was saved above.     */
	
	/* First, read the cpsr value and load it to the SPSR. The */
	/* reason this is done is because it can automatically     */
	/* loaded into the cpsr with the "^" operator.             */
	
	/* Load SPSR */
	ldr	r0, [r1]  /* Read it */
	msr	spsr, r0  /* Load it */

	/* Now increment the stack by 4 bytes to point to r0 which */
	/* is 4 bytes above cpsr.                                  */
	add	r1, #0x4

	/* Update all the general registers, including the PC      */
	/* which is r15. When the "^" operator is used, the spsr   */
	/* will get loaded into the cpst. The noop's are required  */
	/* to clear the pipeline.                                  */
	
	ldmfd	r1, {r0-r15}^
	nop
	nop
	nop


	
/*---------------------------------------------------------------------------*/
/*                                                                           */
/* Function Name   : irq_mode_init                                           */
/* Description     : Initialize the IRQ mode                                 */
/*                                                                           */	
/* Notes           : This code will go into the IRQ mode to update the stack */
/*                   which is the 2x32-bit array irq_stack which is passed   */
/*                   in r0                                                   */
/*                                                                           */	
/*---------------------------------------------------------------------------*/
.globl  irq_mode_init
irq_mode_init:
	/* Go into INT Mode */	
	mrs	r1, cpsr
	bic	r1, r1, #0x01 
	msr	cpsr_c, r1	

	/* Update the Stack Pointer (r13) */
	mov	r13, r0
	
	/* Go back into the non-INT mode (SVC Mode) */
	orr	r1, r1, #0x1
	msr	cpsr_c, r1
	mov	r15, r14
	nop
	nop
	nop


/*---------------------------------------------------------------------------*/
/*                                                                           */
/* Function Name   : cpu_enable_ints                                         */
/* Description     : Start All Interrupts                                    */
/*                                                                           */	
/* Notes           :                                                         */
/*                                                                           */
/*---------------------------------------------------------------------------*/
.globl  cpu_enable_ints
cpu_enable_ints:
	msr	cpsr_c, #0x13      /* Enable the IRQ and FIQ interrupts */
	mov	r15, r14
	nop
	nop
	nop

	
/*---------------------------------------------------------------------------*/
/*                                                                           */
/* Function Name   : cpu_disable_ints                                        */
/* Description     : Stop All Interrupts                                     */
/*                                                                           */
/* Notes           :                                                         */
/*                                                                           */
/*---------------------------------------------------------------------------*/
.globl  cpu_disable_ints
cpu_disable_ints:
	msr	cpsr_c, #0xd3
	mov	r15, r14
	nop
	nop
	nop



