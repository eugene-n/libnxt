.section ".bootstrap"

/*
 * Definition of a few constants used in the interrupt dispatchers.
 */
#define AIC_BASE 0xFFFFF000 /* Base of the AIC register bank. */
#define AIC_IVR 0x100   /* Offset of the IVR register within the AIC */
#define AIC_FVR 0x104   /* Offset of the FVR register within the AIC. */
#define AIC_EOICR 0x130 /* Offset of the EOICR register within the AIC. */

#define MC_BASE 0xFFFFFF00 /* Base of the MC register bank. */
#define MC_REMAP 0x0       /* Offset of the RCR register within the MC. */

#define FIQ_MASK 0x6    /* FIQ disable control bit in the CPSR. */
#define IRQ_MASK 0x7    /* IRQ disable control bit in the CPSR. */

#define MODE_FIQ 0x11   /* FIQ mode. */
#define MODE_IRQ 0x12   /* IRQ mode. */
#define MODE_SVC 0x13   /* Supervisor mode. */

#define RAM_START 0x00200000 /* The first address of RAM. */
#define RAM_END 0x002FFFFF  /* The last address of RAM. */
#define IRQ_STACK_SIZE 0x60 /* The size of the IRQ mode stack: 3*4*8 bytes. */

/*
 * The following is the interrupt vector block of the system.
 */
reset:		b init_reset	/* Reset */
illegal_hdl:	b illegal_hdl 	/* Illegal instruction */
swi_hdl:	b swi_hdl	/* No SWI handling */
prefetch_hdl:	b prefetch_hdl	/* Prefetch abort */
data_hdl:	b data_hdl	/* Data abord */
reserved_hdl:	b reserved_hdl	/* Reserved vector */
irq_hdl:	b irq_dispatch  /* IRQ */
fiq_hdl:			/* FIQ */


/*
 * This is the FIQ handler. It simply wraps the registered FIQ
 * handling routine in the required exception handling code.
 */

	/* Save R0 to an FIQ banked register, replace it with the AIC
 	 * register base address, and switch into supervisor (SVC) mode.
	 */
	mov r9, r0
	ldr r0, [r8, #AIC_FVR]
	msr CPSR_c, #(IRQ_MASK | FIQ_MASK | MODE_SVC)

	/* Now that we have a real stack, save the other registers
	 * we're responsible for saving (according to the ATPCS), and
	 * branch to the FIQ handler.
	 */
	stmfd sp!, {r1-r3, ip, lr}
	mov r14, pc
	bx r0

	/* Restore stacked registers, switch back into FIQ mode and
	 * resume execution.
	 */
	ldmfd sp!, {r1-r3, ip, lr}
	msr CPSR_c, #(IRQ_MASK | FIQ_MASK | MODE_FIQ)
	mov r0, r9
	subs pc, lr, #4


/*
 * IRQ exception dispatcher. This code runs when the CPU detects an
 * interrupt coming from the AIC. It switches to supervisor mode and
 * dispatches executions to the registered handler.
 */
irq_dispatch:

	/* Save the return address and return processor mode registers
	 * on the IRQ stack. This is needed to allow nested interrupts,
	 * where a higher-priority interrupt halts execution of a lower
	 * priority interrupt. Also save r0, as we're going to need a
	 * scratch register.
	 */
	sub lr, lr, #4
	stmfd sp!, {lr}
	mrs lr, SPSR
	stmfd sp!, {lr}
	stmfd sp!, {r0}

	/* Get the IVR value. */
	ldr lr, =AIC_BASE
	ldr r0, [lr, #AIC_IVR]

	/* If we're in Protected mode (usually for JTAG debugging), we
	 * need to write back to the IVR register to tell the AIC it
	 * can dispatch other higher priority interrupts again.
	 * interrupt line. In normal mode, this has no effect, so we can
	 * safely do it.
	 */
	str lr, [lr, #AIC_IVR]

	/* Switch to Supervisor mode and reenable IRQ and FIQ handling. */
	msr CPSR_c, #MODE_SVC

	/* Dispatch the IRQ to the registed handler. */
	stmfd sp!, {r1-r3, ip, lr}
	mov lr, pc
	bx r0

	/* Restore registers and switch back to IRQ mode. */
	ldmfd sp!, {r1-r3, ip, lr}
	msr CPSR_c, #(IRQ_MASK | MODE_IRQ)
	ldmfd sp!, {r0}

	/* Tell the AIC that the interrupt has been handled. */
	str lr, [lr, #AIC_EOICR]

	/* Restore the SPSR register from the IRQ stack. */
	ldmfd sp!, {lr}
	msr SPSR_cxsf, lr

	/* Return to the normal execution flow. */
	ldmfd sp!, {pc}^


/*
 * Default handlers for the interrupt controller. The board is
 * initialized with interrupts disabled, and these handlers set.
 */
nxt_default_fiq_handler:
	b nxt_default_fiq_handler
nxt_default_irq_handler:
	b nxt_default_irq_handler
nxt_spurious_irq_handler:
	b nxt_spurious_irq_handler
.global nxt_default_fiq_handler
.global nxt_default_irq_handler
.global nxt_spurious_irq_handler

end:	b end

/*
 * This is a marker, used in the payload initialization code to figure
 * out where to copy the payload in ram.
 */
payload_start:
.global payload_start


/*
 * Main initialization. This is where the execution starts when the
 * board is powered on.
 */
init_reset:
	/* Set up the supervisor mode stack at the top of RAM. */
	ldr sp, =RAM_END

	/* Call the low-level initialization code. Once this has run,
	 * we're at 48MHz, and the AIC has valid (if useless) exception
	 * handlers registered.
	 */
	bl nxt_low_level_init

	/* Switch to FIQ mode and set the r8 register with the AIC
	 * register base address.
	 */
	msr CPSR_c, #(IRQ_MASK | FIQ_MASK | MODE_FIQ)
	ldr r8, =AIC_BASE

	/* Switch to IRQ mode and set up the IRQ stack at the very top
	 * of ram. Given our IRQ dispatch code, we need 3 words per
	 * priority level, so a stack of 3*4*8 bytes.
	 */
	msr CPSR_c, #(IRQ_MASK | FIQ_MASK | MODE_IRQ)
	ldr r0, =RAM_END
	mov sp, r0
	sub r0, r0, #IRQ_STACK_SIZE

	/* Go back to Supervisor mode and set up the Supervisor stack
	 * to start at the top of the IRQ stack.
	 *
	 *
	 * Note that we also enable interrupts at this point, one
	 * instruction before the SVC stack is set up. In theory, this is
	 * a problem, because we might have an interrupt before we're
	 * ready to handle it. In practice though, the low level init
	 * function explicitely shut down all interrupts in the AIC, so
	 * we're safe.
	 */
	msr CPSR_c, #MODE_SVC
	mov sp, r0

	/* Copy the interrupt vectors and interrupt dispatchers into
	 * ram. We don't copy the init_reset method, as we're going to
	 * put the actual application payload there.
	 */
	ldr r0, =RAM_START
	mov r1, #0
	ldr r2, =payload_start
	bl init_memcpy

	/* Overwrite the reset vector to point to itself. We don't yet
	 * support warm reboots, so we should just crash if it's
	 * attempted.
	 */
	ldr r1, =RAM_START
	str r1, [r1]

	/* Copy the application payload to ram. For this, we use the
	 * handy symbols that ld provides us, that give us the address in
	 * flash where the payload is, and the total size of the payload.
	 *
	 * We load the payload at the address payload_start, which is
	 * where ld relocated the addresses to be.
	 *
	 * Note that this overwrites the code we're currently in, so
	 * as soon as we remap the bottom section from flash to ram, the
	 * crt0 initialization code will be lost. No biggie, we can still
	 * get to it by hitting flash directly.
	 */
	ldr r1, =__b_load
	ldr r2, =__load_size
	bl init_memcpy

	/* Initialize the BSS. Again, we use handy symbols that ld
	 * defines for us, that specify the size of the BSS section.
	 */
	mov r1, #0
	ldr r2, =__bss_size
	bl init_memset

	/* We're almost ready to boot the kernel. We first set up all
	 * the registers, getting ready for the remap. Once the remap
	 * command has executed, we have to banch to load_main before the
	 * pipeline empties, otherwise we'll fall down into the
	 * application payload and crash.
	 */
	ldr lr, =end
	ldr ip, =main
	ldr r4, =MC_BASE
	mov r5, #1

	/* Execute the remap, and branch into the kernel. */
	str r5, [r4, #MC_REMAP]
	bx ip
