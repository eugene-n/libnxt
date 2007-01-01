SECTIONS
{
  . = 0x00000000;
  . = ALIGN(4);
  /* start = 0x00000000; */
  .bootstrap : { crt0.o(.bootstrap) crt0_c.o(.text) }
  .text : { *(.text) }
  .data : { *(.data*) }
  .bss : { *(.bss*) }
}
