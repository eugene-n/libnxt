include environment.mak

TARGET = nxtos

S_SOURCES = crt0_s.S vectors.s
C_SOURCES = main.c aic.c sys_timer.c

S_OBJECTS = $(S_SOURCES:.S=.o)
C_OBJECTS = $(C_SOURCES:.c=.o)

GCC=$(TOOL_PREFIX)gcc$(TOOL_SUFFIX)
AS=$(TOOL_PREFIX)as
LD=$(TOOL_PREFIX)ld
OBJCOPY=$(TOOL_PREFIX)objcopy
OBJDUMP=$(TOOL_PREFIX)objdump

OPTIMIZE=-O2

CCFLAGS=-ffreestanding -W -Wall -Werror $(OPTIMIZE) -mcpu=arm7tdmi -mapcs -mthumb-interwork -Isupport -Icrt0
ASFLAGS=-W -Wall -Werrror $(OPTIMIZE) -Wa,-mcpu=arm7tdmi,-mapcs-32,-mthumb-interwork,-mfpu=softfpa
LDFLAGS=-N -nostdlib $(OPTIMIZE) -T crt0/nxt.lds -Lcrt0

all: estorm.bin

estorm.bin: estorm.elf
	$(OBJCOPY) -O binary -R .bss $< $@
	$(OBJDUMP) --disassemble-all -b binary -m arm7tdmi $@ > $@.asm

estorm.elf: $(S_OBJECTS) $(C_OBJECTS) crt0/crt0.o crt0/nxt.lds
	$(LD) -o $@ $(S_OBJECTS) $(C_OBJECTS) $(LDFLAGS)
	$(OBJDUMP) --disassemble-all -m arm7tdmi $@ > $@.asm

%.o: %.c
	$(GCC) $(CCFLAGS) -c -o $@ $<

%.o: %.S
	$(GCC) $(ASFLAGS) -c -o $@ $<

clean:
	rm -f *.o *.asm *.bin *.elf *~
