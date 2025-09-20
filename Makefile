AS      	= as
ASFLAGS 	= -32

CC      	= gcc
CFLAGS  	= -m32 -ffreestanding -fno-stack-protector -fno-pic -fno-pie -Wall -Wextra -Iinclude -Iarch/x86/include -c -MMD -MP

LD      	= ld
LDFLAGS 	= -m elf_i386 -T linker.ld

SRCDIR    	= kernel
ARCHDIR   	= arch
BUILDDIR  	= build

KERNEL    	= vmxez

CFILES 		:= $(shell find $(SRCDIR) -name '*.c')

COBJS 		:= $(patsubst $(SRCDIR)/%.c,$(BUILDDIR)/%.o,$(CFILES)) \

BOOTOBJ 	= $(BUILDDIR)/boot.o

all: $(KERNEL)

$(BOOTOBJ): $(ARCHDIR)/x86/boot/boot.s
	@mkdir -p $(dir $@)
	$(AS) $(ASFLAGS) $< -o $@

$(BUILDDIR)/%.o: $(SRCDIR)/%.c
	@mkdir -p $(dir $@)
	$(CC) $(CFLAGS) $< -o $@

$(KERNEL): $(BOOTOBJ) $(COBJS)
	$(LD) $(LDFLAGS) -o $(BUILDDIR)/$@ $(BOOTOBJ) $(COBJS)
	mv $(BUILDDIR)/$@ ./

clean:
	rm -rf $(BUILDDIR) $(KERNEL)

-include $(COBJS:.o=.d)

.PHONY: all clean
