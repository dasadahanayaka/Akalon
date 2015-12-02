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
/* Description     : IA32 assembly language code                             */
/*                                                                           */	
/* Notes           :                                                         */
/*                                                                           */	
/* Calling a C function from IA32 assembler code...                          */
/*                                                                           */
/*        Last Param                                                         */
/*           ...                                                             */
/*           ...                                                             */
/*        First Param                                                        */
/*        Return Addr                                                        */
/*                                                                           */
/* The return is in register eax                                             */
/*                                                                           */	
/* - In the ia32 architecture, the stack growns downwards.                   */
/* - For a push, esp is first decremented by 4 and the value then pushed.    */
/* - For a pop, data is first pop'ed and then esp is incremented by 4.       */
/*                                                                           */
/*---------------------------------------------------------------------------*/


#define ASM        1

/*---------------------------------------------------------------------------*/
/* The macro IA32_INT contains the code that is executed when an interrupt   */
/* occures. In other words, it's the interrupt vector code.                  */
/*                                                                           */
/* This is the story behind it...                                            */
/*                                                                           */
/*  0) When an interrupt occures, the processor pushes the eflags, cs and    */
/*     eip onto the stack and then starts executing this Macro. So when in   */
/*     this code, the stack looks like this..                                */
/*                                                                           */
/*        eflags                                                             */
/*        cs            (extended to 32 bits)                                */
/*        ip            <-- esp                                              */
/*                                                                           */
/*  1) The Macro then pushes all the general registers onto the stack. The   */
/*     stack will now look like...                                           */
/*                                                                           */
/*        eflags                                                             */
/*        cs                                                                 */
/*        eip                                                                */
/*        eax                                                                */
/*        ecx                                                                */
/*        edx                                                                */
/*        ebx                                                                */
/*        <esp>         <-- points to eip                                    */
/*        ebp                                                                */
/*        esi                                                                */
/*        edi           <-- esp                                              */
/*                                                                           */
/*  2) It will now push the interrupt number onto the stack and will call    */
/*     int_run(). Just before the call to int_run(), the stack will look     */
/*     like...                                                               */
/*                                                                           */
/*        eflags                                                             */
/*        cs                                                                 */
/*        eip                                                                */
/*        eax                                                                */
/*        ecx                                                                */
/*        edx                                                                */
/*        ebx                                                                */
/*        <esp>         <-- points to eip                                    */
/*        ebp                                                                */
/*        esi                                                                */
/*        edi                                                                */
/*        int_num       <-- esp                                              */
/*                                                                           */
/*  3) int_run() will call the registered ISR, and will check if another     */
/*     higher prioroty task got into a ready state. If it did, then          */
/*     int_run() will send back that task's stack pointer in register eax.   */
/*                                                                           */
/*     If no higher priority task became ready, then eax will be zero, and   */
/*     this macro will pop back the saved general registers and will execute */
/*     the iret instruction and continue to execute from the point the task  */
/*     got interrupted.                                                      */
/*                                                                           */
/*  4) If int_run() made a higher priority task ready, then save the higher  */
/*     priority task's stack pointer (in eax) onto the stak. In this case,   */
/*     a move is used instead of a push (for perfomance). The stack now      */
/*     looks like...                                                         */
/*                                                                           */
/*        eflags                                                             */
/*        cs                                                                 */
/*        eip                                                                */
/*        eax                                                                */
/*        ecx                                                                */
/*        edx                                                                */
/*        ebx                                                                */
/*        <esp>         <-- points to eip                                    */
/*        ebp                                                                */
/*        esi                                                                */
/*        edi                                                                */
/*        int_num       <-- esp                                              */
/*        eax           <-- Higher priority task's stack pointer             */
/*                                                                           */
/*  4) We are now ready to save the interrupted task's context. Here is what */
/*     needs to happen.                                                      */
/*                                                                           */
/*     a. Move eflags to where cs is lcoated                                 */
/*     b. Move eip to where eax is located                                   */
/*     c. Add 4 to esp (it will then point to edi)                           */
/*     d. popall                                                             */
/*     e. Add 4 to esp (it will then point to eflags)                        */
/*     f. pushal (this will save the current task's context on it's stack)   */
/*                                                                           */
/*     This is how the stack looks like before and after stage 4             */
/*                                                                           */
/*        -- Before --            -- After --                                */
/*           eflags                  eip                                     */
/*           cs                      eflags                                  */
/*           eip                     eax                                     */
/*           eax                     ecx                                     */
/*           ecx                     edx                                     */
/*           edx                     ebx                                     */
/*           ebx                     <esp>       <-- points to eflags        */
/*           <esp>                   ebp                                     */
/*           ebp                     esi                                     */
/*           esi                     edi         <-- esp                     */
/*           edi                     xx                                      */
/*           int_num                 int_num                                 */
/*           eax                     eax                                     */
/*                                                                           */
/*  5) Now the interrupted task's tcb needs to be updated with the new stack */
/*     pointer. This is done by calling the c function save_task_state()     */
/*     which takes the new stack location as it's first argument. Push the   */
/*     the new stack pointer, but push it below the eax value which contains */
/*     the stack location of the higher priority task. To do this, subtract  */
/*     12 bytes from the stack pointer. When stage 5 is complete, the        */
/*     interrupted task's entire context would have been saved. Before the   */
/*     the function stave_task_state() is called, the stack will look like.. */
/*                                                                           */
/*        eip                                                                */
/*        eflags                                                             */
/*        eax                                                                */
/*        ecx                                                                */
/*        edx                                                                */
/*        ebx                                                                */
/*        <esp>         <-- Points to eflags                                 */
/*        ebp                                                                */
/*        esi                                                                */
/*        edi                                                                */
/*        xx                                                                 */
/*        int_num       <-- esp                                              */
/*        eax           <-- Higher priority task's stack pointer             */
/*        eax           <-- esp. Points to the interrupted task's stack end  */
/*                          which contains it's context. In other words, it  */
/*                          points to where edi is located.                  */
/*                                                                           */
/*  6) Now load the higher priority task's stack. The location is 4 bytes    */
/*     above the current stack pointer. Move this to esp and then do a popal */
/*     to load all the general registers. 	                             */
/*                                                                           */
/*  7) The higher priority task's context is now loaded, but it's stack is   */
/*     not in a way that the iret instruction can be executed. Therefore,    */
/*     the stack is fixed according to the followin...                       */
/*                                                                           */
/*       Before fixing stack            After fixing stack                   */
/*                                                                           */
/*        eip                           eflags                               */
/*        eflags  <-- esp               cs (=0x8)                            */
/*                                      eip        <-- esp                   */
/*                                                                           */
/*     To achive the above, the following is done...                         */
/*                                                                           */
/*     a. Move eip to 4 bytes below the stack pointer                        */
/*                                                                           */
/*        eip                                                                */
/*        eflags  <-- esp                                                    */
/*        eip                                                                */
/*                                                                           */
/*     b. Move eflags 4 bytes above the stack pointer                        */
/*                                                                           */
/*        eflags                                                             */
/*        eflags  <-- esp                                                    */
/*        eip                                                                */
/*                                                                           */
/*     c. Store 0x8 (cs) at the stack pointer. 0x8 is the code segment       */
/*        offset in the GDT.                                                 */
/*                                                                           */
/*        eflags                                                             */
/*        0x8     <-- esp                                                    */
/*        eip                                                                */
/*                                                                           */
/*     d. Move esp to point to eip                                           */
/*                                                                           */
/*        eflags                                                             */
/*        0x8                                                                */
/*        eip     <-- esp                                                    */
/*                                                                           */
/*  8) To implement (7) we used register ebx, which trashed the value of ebx */
/*     that was loaded in (6) as part of the popal. Fortunetly, we still     */
/*     have ebx on the stack 12 bytes below the stack pointer. We load it    */
/*     and get out of the processor's interrupt context by an iret           */
/*     instruction. The interrupt has now been processed.                    */
/*                                                                           */
/*---------------------------------------------------------------------------*/
#define IA32_INT(int_num)        \
	/* Stage 1 */            \
	pushal                  ;\
                                 \
	/* Stage 2 */            \
	movl int_num, %eax      ;\
	pushl %eax              ;\
	call int_run            ;\
	                         \
	/* Stage 3 */            \
	cmp  $0x0, %eax         ;\
	jz   1f                 ;\
	                         \
	/* Stage 4 */            \
	movl %eax, -4(%esp)     ;\
	                         \
	/* Stage 4.a eflags */   \
	movl 44(%esp), %eax     ;\
	movl %eax, 40(%esp)     ;\
	                         \
	/* Stage 4.b eip */      \
	movl 36(%esp), %eax     ;\
	movl %eax, 44(%esp)     ;\
	                         \
	/* Stage 4.c/d popal */  \
	addl $0x4, %esp         ;\
	popal                   ;\
	                         \
	/* Atage 4.e/f pushal */ \
	addl $0x4, %esp         ;\
	pushal                  ;\
	                         \
	/* Context saved */      \
                                 \
	/* Stage 5 */            \
	movl  %esp, %eax        ;\
	subl  $0xc, %esp        ;\
	pushl %eax              ;\
	call  save_task_state   ;\
	                         \
	/* Stage 6. Load new */  \
	/* stack.            */  \
        addl  $0x4, %esp        ;\
	popl  %esp              ;\
	                         \
	/* Read new Regs */      \
	popal                   ;\
	                         \
	/* Stage 7.a (eip) */    \
	movl  4(%esp), %ebx     ;\
        movl  %ebx, -4(%esp)    ;\
	                         \
	/* Stage 7.b (eflags) */ \
	movl  (%esp), %ebx      ;\
	movl  %ebx, 4(%esp)     ;\
	                         \
        /* Stage 7.c (CS=0x8) */ \
        movl  	$0x8, %ebx      ;\
	movl	%ebx, (%esp)    ;\
	                         \
	/* Stage 7.d (esp=eip) */\
	subl	$0x4, %esp      ;\
	                         \
	/* Stage 8 (load ebx) */;\
	movl	-12(%esp), %ebx ;\
	iret                    ;\
	                         \
	/* Stage 3. Continue */  \
	/* with interrupted  */  \
	/* task.             */  \
1:	addl 	$0x4, %esp      ;\
        popal			;\
	iret                    ;


	
        .text
	
/*---------------------------------------------------------------------------*/
/*                                                                           */
/* Function Name   : cpu_task_first                                          */
/* Description     : Spawn the first task (i.e. the Idle Task)               */
/*                                                                           */
/* Notes           : Originally this code was part of the cpu_task_switch()  */
/*                   function, but was removed from it to increase it's      */
/*                   performance. How ? When this code was part of the       */
/*                   function cpu_task_switch(), the special case was        */
/*                   checked on every task switch, while the special case    */
/*                   happened only at init time when the Idle task was       */
/*                   spawned.                                                */
/*                                                                           */
/*---------------------------------------------------------------------------*/
.globl  cpu_task_first 
cpu_task_first:
        .align 4

        /* The stack is currently...                               */
        /*                                                         */
        /*     1st Param    <-- Stack of Idle Task                 */
        /*     Ret Address  <-- esp (points to os_init)            */
        /*                                                         */
        /* The stack of the Idle Task looks like...                */
        /*                                                         */
        /*   0xb1gb00da (End of the Task's Task).                  */
        /*   4th input param (arg2)                                */
        /*   3rd input param (arg1)                                */
        /*   2nd input param (arg0)                                */
        /*   1st input param (Task Entry Point)                    */
        /*   Dummy Return Address                                  */
        /*   eip   <-- Address of task_start()                     */
        /*   eflags                                                */
        /*   eax                                                   */
        /*   ecx                                                   */
        /*   edx                                                   */
        /*   ebx                                                   */
        /*   <esp> <-- Points to eflags                            */
        /*   ebp                                                   */
        /*   esi                                                   */
        /*   edi   <-- task->stack_ptr                             */

        movl	4(%esp), %esp /* Load esp */

        popal
        popfl   /* Now the stack points to task_start() */
	ret     /* Go to task_start()                   */   



/*---------------------------------------------------------------------------*/
/*                                                                           */
/* Function Name   : cpu_task_switch                                         */
/* Description     : Context switch code                                     */
/*                                                                           */
/* Notes           : Saves the currently running task and switches to the    */
/*                   new task.                                               */
/*---------------------------------------------------------------------------*/
	
.globl  cpu_task_switch
cpu_task_switch :	
        .align 4
	
        /* The stack is currently...                               */
        /*                                                         */
        /*     2nd Param    <-- Stack pointer of the new task      */
        /*     1st Param    <-- Location to save current task      */
        /*     Ret Address  <-- esp                                */
        /*                                                         */

        /* Save the currently running task's context on it's stack */

        pushfl
        pushal

        /* Now the stack looks like this...                        */
        /*                                                         */
        /*     2nd Param    <-- 44 Bytes away from esp             */
        /*     1st Param    <-- 40 Bytes away from esp             */
        /*     Ret Address                                         */
        /*     eflags                                              */
        /*     eax                                                 */
        /*     ecx                                                 */
        /*     edx                                                 */
        /*     ebx                                                 */
        /*     <esp>        <-- Points to Ret Address              */
        /*     ebp                                                 */
        /*     esi                                                 */
        /*     edi          <-- esp                                */
        /*                                                         */
        /* The currently running task's context (registers) are    */
        /* now saved on it's stack.                                */

        /* Now save the new stack pointer to stack_ptr variable of */
        /* the tcb. This location was provided by the caller and   */
        /* is located 40 bytes above the location of the current   */
        /* stack pointer.                                          */
	
        movl	40(%esp), %eax
        movl	%esp, (%eax)

        /* At this point, we have saved off the context (regs) of  */
        /* the currently running task's and have saved it's stack  */
	/* pointer.                                                */

	/* Now the new task's context can be loaded. The location  */
	/* (i.e it's stack) was provided by the caller and is      */
	/* located 44 bytes above the current stack location. Load */
	/* the new stack and jump to where the general registers   */
        /* and the eflags register is located. The "ret" will then */
	/* take where the new task previously ended.               */
	
        movl	44(%esp), %esp /* Load the new Stack */

        popal
        popfl	/* Stack points to the start/restart location */   
        ret  	/* Go to the start/restart location */        


/*---------------------------------------------------------------------------*/
/*                                                                           */
/* Function Name   : loadIDT                                                 */
/* Description     : Loads the IDT into the Microprocessor.                  */
/* Notes           :                                                         */
/*                                                                           */
/*---------------------------------------------------------------------------*/
.globl  loadIDT
loadIDT:
        lidt 	idtLoc
        ret


/*---------------------------------------------------------------------------*/
/*                                                                           */
/* Function Name   : nullISR                                                 */
/* Description     : Null ISR...da !!                                        */
/* Notes           :                                                         */
/*                                                                           */
/*---------------------------------------------------------------------------*/
.globl  nullISR
nullISR:
        iret


/*---------------------------------------------------------------------------*/
/*                                                                           */
/* Function Name   : intXXISR                                                */
/* Description     : Individual ISRs                                         */
/* Notes           :                                                         */
/*                                                                           */
/*---------------------------------------------------------------------------*/
.globl  int32ISR
int32ISR:
        .align 4
        IA32_INT($0x20)

.globl  int33ISR	
int33ISR:
        .align 4
        IA32_INT($0x21)

.globl  int36ISR	
int36ISR:
        .align 4
        IA32_INT($0x24)



/*---------------------------------------------------------------------------*/
/*                                                                           */
/* Function Name   : cpu_enable_ints                                         */
/* Description     : Start All Interrupts                                    */
/* Notes           :                                                         */
/*                                                                           */
/*---------------------------------------------------------------------------*/
.globl  cpu_enable_ints
cpu_enable_ints:
        sti
        ret
	
/*---------------------------------------------------------------------------*/
/*                                                                           */
/* Function Name   : cpu_disable_ints                                        */
/* Description     : Stop All Interrupts                                     */
/* Notes           :                                                         */
/*                                                                           */
/*---------------------------------------------------------------------------*/
.globl  cpu_disable_ints
cpu_disable_ints:
        cli
        ret


	
/*---------------------------------------------------------------------------*/
/*                                                                           */
/* Function Name   : cpuid                                                   */
/* Description     : Get the CPUID                                           */
/* Notes           :                                                         */
/*                                                                           */
/*---------------------------------------------------------------------------*/
.globl	cpuid
cpuid:
	pushl   %ebx
	pushl	%ecx
	pushl	%edx

	movl	16(%esp), %ebx
	movl	(%ebx), %eax
	cpuid

	pushl	%eax
	pushl	%ebx

	/* EAX */
	movl	24(%esp), %eax
	movl	4(%esp), %ebx
	movl	%ebx, (%eax)

	/* EBX */
	movl	28(%esp), %eax
	movl	(%esp), %ebx
	movl	%ebx, (%eax)

	/* ECX */
	movl	32(%esp), %eax
	movl	%ecx, (%eax)

	/* EDX */
	movl	36(%esp), %eax
	movl	%edx, (%eax)	

	popl	%eax
	popl	%eax
	
	
	popl	%edx
	popl	%ecx
	popl	%ebx
	ret

