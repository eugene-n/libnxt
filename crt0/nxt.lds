SECTIONS
{
  . = 0x00000000;
  . = ALIGN(4);
  .bootstrap : { crt0.o(.bootstrap) }
  __b_load = .;
  . = payload_start;
  .text . : AT(LOADADDR(.bootstrap) + SIZEOF(.bootstrap)) {
    *(.text*)
    *(.data*)
    *(.rodata*)
  }
  __load_size = SIZEOF(.text);
  .bss . : AT(LOADADDR(.text) + SIZEOF(.text)) { *(.bss*) }
  __bss_size = SIZEOF(.bss);

  /DISCARD/ : {
  	    *(.note*)
	    *(.indent)
	    *(.comment)
	    *(.stab)
	    *(.stabstr)
  }

}
