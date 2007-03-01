SECTIONS
{
  . = 0x00000000;
  . = ALIGN(4);
  .bootstrap : { crt0_s.o(.bootstrap) crt0_c.o(.text) }

  /DISCARD/ : {
  	    *(.text*)
	    *(.data*)
	    *(.rodata*)
	    *(.bss*)
  	    *(.note*)
	    *(.indent)
	    *(.comment)
	    *(.stab)
	    *(.stabstr)
  }
}
