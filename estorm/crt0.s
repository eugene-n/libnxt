.section ".bootstrap"

 /* The following is the interrupt vector block of estorm when boots. */

reset:		b estorm_crt0	/* Reset */
illegal_hdl:	b illegal_hdl 	/* Illegal instruction */
swi_hdl:	b swi_hdl	/* No SWI handling */
prefetch_hdl:	b prefetch_hdl	/* Prefetch abort */
data_hdl:	b data_hdl	/* Data abord */
reserved_hdl:	b reserved_hdl	/* Reserved vector */
irq_hdl:	b irq_hdl       /* IRQ */
fiq_hdl:	b fiq_hdl	/* FIQ */

estorm_crt0:	ldr sp, =0x002FFFFF /* Place stack at very top of ram */
		bl estorm_boot	/* Start the monitor. */
estorm_end:	b estorm_end	/* Crash if monitor ever returns */
