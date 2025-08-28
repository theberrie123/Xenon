AS = as
ASFLAGS = -32

CC = gcc
CFLAGS = -m32 -ffreestanding -fno-stack-protector -fno-pic -fno-pie \
         -Wall -Wextra -Iinclude -Iarch/x86/include -c -MMD -MP

LD = ld
LDFLAGS = -m elf_i386 -T linker.ld

SRCDIR = kernel
ARCHDIR = arch
BUILDDIR = build

KERNEL = vmxez

CFILES := $(wildcard $(SRCDIR)/*.c)
COBJS := $(patsubst $(SRCDIR)/%.c,$(BUILDDIR)/%.o,$(CFILES))
BOOTOBJ = $(BUILDDIR)/boot.o

all: $(BUILDDIR) $(KERNEL)
	@rm -rf $(BUILDDIR)

# Ensure build directory exists
$(BUILDDIR):
	mkdir -p $@

# Assemble boot sector
$(BOOTOBJ): $(ARCHDIR)/x86/boot/boot.s
	$(AS) $(ASFLAGS) $< -o $@

# Compile C files
$(BUILDDIR)/%.o: $(SRCDIR)/%.c
	$(CC) $(CFLAGS) $< -o $@

# Link kernel inside build/
$(KERNEL): $(BOOTOBJ) $(COBJS)
	$(LD) $(LDFLAGS) -o $(BUILDDIR)/$@ $(BOOTOBJ) $(COBJS)
	mv $(BUILDDIR)/$@ ./   # move final binary to main folder


-include $(COBJS:.o=.d)

.PHONY: all
