
        .arm

	.globl reset
        .globl Abort_Handler



        .section .text.reset
	.align 4
reset:
/* Exception vectors (should be a branch to be detected as a valid code by the rom */
_exception_vectors:
        b _program_start           
        b Undefined_Handler       
        b SWI_Handler             
        b Prefetch_Handler        
        b Abort_Handler           
        .word   0                       
        b IRQ_Handler             
        b FIQ_Handler             


        .section .text._program_start
	.align 4
_program_start:

#define MODE_MSK 0x1F           

#define USR_MODE 0x10            
#define FIQ_MODE 0x11        
#define IRQ_MODE 0x12            
#define SVC_MODE 0x13            
#define ABT_MODE 0x17            
#define UND_MODE 0x1B         
#define SYS_MODE 0x1F            
#define I_BIT    (0x1 << 7)
#define F_BIT    (0x1 << 6)

        // Mask interrupts
        cpsid   if

        // Reset SCTLR Settings
        mrc     p15, 0, r0, c1, c0, 0   // Read CP15 System Control register
        bic     r0, r0, #(0x1 << 12)    // Clear I bit 12 to disable I Cache
        bic     r0, r0, #(0x1 << 2)     // Clear C bit  2 to disable D Cache
        bic     r0, r0, #0x1            // Clear M bit  0 to disable MMU
        bic     r0, r0, #(0x1 << 11)    // Clear Z bit 11 to disable branch perdiction
        bic     r0, r0, #(0x1 << 13)    // Clear V bit 13 to disable hivecs
        mcr     p15, 0, r0, c1, c0, 0   // write value back to CP15 System Control register
        isb

        // Configure ACTLR
        mrc     p15, 0, r0, c1, c0, 1   // Read CP15 Auxiliary Control Register
        orr     r0, r0, #(0x1 << 1)     // Enable L2 prefetch hit (UNK/WI since r4p1)
        mcr     p15, 0, r0, c1, c0, 1   // Write CP15 Auxiliary Control Register

        // Set Vector Base Address Register (VBAR) to point to this application's vector table
        ldr     r0, =_exception_vectors
        mcr     p15, 0, r0, c12, c0, 0

        // Put any cores other than 0 to sleep
        mrc     p15, 0, r0, c0, c0, 5   // Read MPIDR
        ubfx	r0, r0, #0, #12
        cmp	r0, #0
        beq     core0_start
        cmp	r0, #1
        beq     core1_start
        cmp	r0, #2
        beq     core2_start
        cmp	r0, #3
        beq     core3_start

core0_start:
        // Set up stack for each exceptional mode
        mrs     r0, cpsr                // Original PSR value

        // Set up the svc stack pointer.
        bic     r0, r0, #MODE_MSK       // Clear the mode bits
        orr     r0, r0, #SVC_MODE       // Set SVC mode bits
        msr     cpsr_c, r0              // Change the mode
        ldr     r1, =SVC_STACK_CORE0    // End of SVC_STACK
        bic     sp,r1,#0x7              // Make sure SP is 8 aligned

        // Set up the interrupt stack pointer.
        bic     r0, r0, #MODE_MSK       // Clear the mode bits
        orr     r0, r0, #IRQ_MODE       // Set IRQ mode bits
        msr     cpsr_c, r0              // Change the mode
        ldr     r1, =IRQ_STACK_CORE0    // End of IRQ_STACK
        bic     sp,r1,#0x7              // Make sure SP is 8 aligned

        // Set up the fast interrupt stack pointer.
        bic     r0, r0, #MODE_MSK       // Clear the mode bits
        orr     r0, r0, #FIQ_MODE       // Set FIR mode bits
        msr     cpsr_c, r0              // Change the mode
        ldr     r1, =FIQ_STACK_CORE0    // End of FIQ_STACK
        bic     sp,r1,#0x7              // Make sure SP is 8 aligned

        // Set up the normal stack pointer.
        bic     r0 ,r0, #MODE_MSK       // Clear the mode bits
        orr     r0 ,r0, #SYS_MODE       // Set System mode bits
        msr     cpsr_c, r0              // Change the mode
        ldr     r1, =CSTACK_CORE0       // End of CSTACK
        bic     sp,r1,#0x7              // Make sure SP is 8 aligned

        bl      SMPLowLiveInit

/* Copy the data segment initializers from flash to SRAM */
        mov	r1, #0
        bl	LoopCopyDataInit

CopyDataInit:
	ldr	r3, =_sidata
	ldr	r3, [r3, r1]
	str	r3, [r0, r1]
	add	r1, r1, #4

LoopCopyDataInit:
	ldr	r0, =_sdata
	ldr	r3, =_edata
	add	r2, r0, r1
	cmp	r2, r3
	bcc	CopyDataInit
	ldr	r2, =_sbss
	bl	LoopFillZerobss
/* Zero fill the bss segment. */
FillZerobss:
	mov	r3, #0
	str	r3, [r2], #4

LoopFillZerobss:
	ldr	r3, = _ebss
	cmp	r2, r3
	bcc	FillZerobss

        bl      SystemInit
	//ldr	r0, = main
        //bl      __libc_start_main
        cpsie   if
	bl      main
        bx      r0  //JUMP

core1_start:
        // Set up stack for each exceptional mode
        mrs     r0, cpsr                // Original PSR value

        // Set up the svc stack pointer.
        bic     r0, r0, #MODE_MSK       // Clear the mode bits
        orr     r0, r0, #SVC_MODE       // Set SVC mode bits
        msr     cpsr_c, r0              // Change the mode
        ldr     r1, =SVC_STACK_CORE1    // End of SVC_STACK
        bic     sp,r1,#0x7              // Make sure SP is 8 aligned

        // Set up the interrupt stack pointer.
        bic     r0, r0, #MODE_MSK       // Clear the mode bits
        orr     r0, r0, #IRQ_MODE       // Set IRQ mode bits
        msr     cpsr_c, r0              // Change the mode
        ldr     r1, =IRQ_STACK_CORE1    // End of IRQ_STACK
        bic     sp,r1,#0x7              // Make sure SP is 8 aligned

        // Set up the fast interrupt stack pointer.
        bic     r0, r0, #MODE_MSK       // Clear the mode bits
        orr     r0, r0, #FIQ_MODE       // Set FIR mode bits
        msr     cpsr_c, r0              // Change the mode
        ldr     r1, =FIQ_STACK_CORE1    // End of FIQ_STACK
        bic     sp,r1,#0x7              // Make sure SP is 8 aligned

        // Set up the normal stack pointer.
        bic     r0 ,r0, #MODE_MSK       // Clear the mode bits
        orr     r0 ,r0, #SYS_MODE       // Set System mode bits
        msr     cpsr_c, r0              // Change the mode
        ldr     r1, =CSTACK_CORE1       // End of CSTACK
        bic     sp,r1,#0x7              // Make sure SP is 8 aligned

        bl      SMPLowLiveInit
        cpsie   if
	bl      main
        bx      r0  //JUMP

core2_start:
        // Set up stack for each exceptional mode
        mrs     r0, cpsr                // Original PSR value

        // Set up the svc stack pointer.
        bic     r0, r0, #MODE_MSK       // Clear the mode bits
        orr     r0, r0, #SVC_MODE       // Set SVC mode bits
        msr     cpsr_c, r0              // Change the mode
        ldr     r1, =SVC_STACK_CORE2    // End of SVC_STACK
        bic     sp,r1,#0x7              // Make sure SP is 8 aligned

        // Set up the interrupt stack pointer.
        bic     r0, r0, #MODE_MSK       // Clear the mode bits
        orr     r0, r0, #IRQ_MODE       // Set IRQ mode bits
        msr     cpsr_c, r0              // Change the mode
        ldr     r1, =IRQ_STACK_CORE2    // End of IRQ_STACK
        bic     sp,r1,#0x7              // Make sure SP is 8 aligned

        // Set up the fast interrupt stack pointer.
        bic     r0, r0, #MODE_MSK       // Clear the mode bits
        orr     r0, r0, #FIQ_MODE       // Set FIR mode bits
        msr     cpsr_c, r0              // Change the mode
        ldr     r1, =FIQ_STACK_CORE2    // End of FIQ_STACK
        bic     sp,r1,#0x7              // Make sure SP is 8 aligned

        // Set up the normal stack pointer.
        bic     r0 ,r0, #MODE_MSK       // Clear the mode bits
        orr     r0 ,r0, #SYS_MODE       // Set System mode bits
        msr     cpsr_c, r0              // Change the mode
        ldr     r1, =CSTACK_CORE2       // End of CSTACK
        bic     sp,r1,#0x7              // Make sure SP is 8 aligned

        bl      SMPLowLiveInit
        cpsie   if
	bl      main
        bx      r0  //JUMP

core3_start:
        // Set up stack for each exceptional mode
        mrs     r0, cpsr                // Original PSR value

        // Set up the svc stack pointer.
        bic     r0, r0, #MODE_MSK       // Clear the mode bits
        orr     r0, r0, #SVC_MODE       // Set SVC mode bits
        msr     cpsr_c, r0              // Change the mode
        ldr     r1, =SVC_STACK_CORE3    // End of SVC_STACK
        bic     sp,r1,#0x7              // Make sure SP is 8 aligned

        // Set up the interrupt stack pointer.
        bic     r0, r0, #MODE_MSK       // Clear the mode bits
        orr     r0, r0, #IRQ_MODE       // Set IRQ mode bits
        msr     cpsr_c, r0              // Change the mode
        ldr     r1, =IRQ_STACK_CORE3    // End of IRQ_STACK
        bic     sp,r1,#0x7              // Make sure SP is 8 aligned

        // Set up the fast interrupt stack pointer.
        bic     r0, r0, #MODE_MSK       // Clear the mode bits
        orr     r0, r0, #FIQ_MODE       // Set FIR mode bits
        msr     cpsr_c, r0              // Change the mode
        ldr     r1, =FIQ_STACK_CORE3    // End of FIQ_STACK
        bic     sp,r1,#0x7              // Make sure SP is 8 aligned

        // Set up the normal stack pointer.
        bic     r0 ,r0, #MODE_MSK       // Clear the mode bits
        orr     r0 ,r0, #SYS_MODE       // Set System mode bits
        msr     cpsr_c, r0              // Change the mode
        ldr     r1, =CSTACK_CORE3       // End of CSTACK
        bic     sp,r1,#0x7              // Make sure SP is 8 aligned

        bl      SMPLowLiveInit
        cpsie   if
	bl      main
        bx      r0  //JUMP

        .section .text.fiq_handler
	.align 4
FIQ_Handler:
        b FIQ_Handler

        .section .text.irq_handler
	.align 4
IRQ_Handler:
        push    {lr}                         /* Save return address+4                                */
        push    {r0-r3, r12}                 /* Push caller save registers                           */

        mrs     r0, spsr                     /* Save SPRS to allow interrupt reentry                 */
        push    {r0}

        bl      IRQ_GetActiveIRQ

        push    {r0}

        cps     #0x13                        /* Change to Supervisor mode to allow interrupt reentry */

        push    {lr}                         /* Save Supervisor lr  */

        bl      IRQ_GetHandler
        cmp	r0, #0
        beq     not_jump_handler
        blx     r0
not_jump_handler:
                                
        pop     {lr}

        cps     #0x12                        /* Back to IRQ mode                                     */

        pop     {r0}

        bl      IRQ_EndOfInterrupt

        pop     {r0}
        msr     spsr_cxsf, r0

        pop     {r0-r3, r12}
        pop     {lr}
        subs    pc, lr, #4

        .section .text.handler
	.align 4
Undefined_Handler:
SWI_Handler:
Prefetch_Handler:
Abort_Handler:
        b Abort_Handler

        .end
