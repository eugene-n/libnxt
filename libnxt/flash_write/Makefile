#all:
#	gcc -Wall -O0 -mcpu=arm7tdmi-s -mapcs -mthumb-interwork -o flash.o flash.c -nostartfiles -nodefaultlibs -nostdlib -Wl,-e,main
#	objcopy -Obinary -j.text flash.o flash.bin
#	objdump --disassemble-all -bbinary -marm7tdmi flash.bin > flash.asm
#

all:
	arm-linux-gcc -W -Wall -O3 -mcpu=arm7tdmi -mapcs -mthumb-interwork -c -o flash.o flash.c
	arm-linux-as --warn -mcpu=arm7tdmi -mapcs-32 -EL -mthumb-interwork -o crt0.o crt0.s
	arm-linux-ld --gc-sections crt0.o flash.o -o flash
	arm-linux-objcopy -O binary flash flash.bin
	arm-linux-objdump --disassemble-all -b binary -m arm7tdmi flash.bin > flash.asm

clean:
	rm -f flash.o flash.bin flash.asm
