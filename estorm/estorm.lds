SECTIONS
{
  . = 0x00000000;
  . = ALIGN(4);
  start = 0x00000000;
  .bootstrap : { *(.bootstrap) }
  .text : { *(.text*) }
  .data : { *(.data*) }
}
