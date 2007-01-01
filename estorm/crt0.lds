SECTIONS
{
  . = 0x00000000;
  . = ALIGN(4);
  start = 0x00000000;
  .text : { *(.bootstrap) *(.text*) }
  .data : { *(.data*) }
  .bss : { *(.bss*) }
}
