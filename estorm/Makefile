#all:
#	gcc -Wall -O0 -mcpu=arm7tdmi-s -mapcs -mthumb-interwork -o flash.o flash.c -nostartfiles -nodefaultlibs -nostdlib -Wl,-e,main
#	objcopy -Obinary -j.text flash.o flash.bin
#	objdump --disassemble-all -bbinary -marm7tdmi flash.bin > flash.asm
#

TOOL_PREFIX=arm-elf-
TOOL_SUFFIX=-4.1.1

GCC=$(TOOL_PREFIX)gcc$(TOOL_SUFFIX)
AS=$(TOOL_PREFIX)as
LD=$(TOOL_PREFIX)ld
OBJCOPY=$(TOOL_PREFIX)objcopy
OBJDUMP=$(TOOL_PREFIX)objdump

all:
	$(GCC) -W -Wall -O2 -Wa,-mcpu=arm7tdmi,-mapcs-32,-mthumb-interwork -c -o crt0.o crt0.S
	$(GCC) -W -Wall -O2 -mcpu=arm7tdmi -mapcs -mthumb-interwork -c -o crt0_c.o crt0_c.c
	$(LD) -N -O2 -T crt0.lds crt0.o crt0_c.o -o crt0
	$(OBJCOPY) -O binary estorm estorm.bin
	$(OBJDUMP) --disassemble-all -b binary -m arm7tdmi estorm.bin > estorm.asm

clean:
	rm -f crt0.o crt0_c.o estorm estorm.bin estorm.asm
