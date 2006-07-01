SECTIONS
{
  . = 0x00200000;
  . = ALIGN(4);
  ENTRY(reset);
  .crt : { crt0.o(.text) }
  .text : { *(EXCLUDE_FILE (*crt0.o) .text) }
  .data : { *(.data) }
  .bss : { *(.bss) }
}
