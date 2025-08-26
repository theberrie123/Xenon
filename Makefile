# Makefile for Xenon kernel (no GRUB, no automatic QEMU)

AS = as
ASFLAGS = -32

CC = gcc
CFLAGS = -m32 -ffreestanding -fno-stack-protector -fno-pic -fno-pie \
         -Wall -Wextra -Iinclude -Iarch/x86/include -c -MMD -MP

LD = ld
LDFLAGS = -m elf_i386 -T linker.ld

SRCDIR = kernel
ARCHDIR = arch
BUILDDIR_OBJ = build/obj
BUILDDIR_BIN = build/bin

KERNEL = $(BUILDDIR_BIN)/vmxez

CFILES := $(wildcard $(SRCDIR)/*.c)
COBJS := $(patsubst $(SRCDIR)/%.c,$(BUILDDIR_OBJ)/%.o,$(CFILES))
BOOTOBJ = $(BUILDDIR_OBJ)/boot.o

DIRS = $(BUILDDIR_OBJ) $(BUILDDIR_BIN)

all: $(KERNEL)

$(DIRS):
	mkdir -p $@

# Assemble boot sector
$(BOOTOBJ): $(ARCHDIR)/x86/boot/boot.s | $(BUILDDIR_OBJ)
	$(AS) $(ASFLAGS) $< -o $@

# Compile C files
$(BUILDDIR_OBJ)/%.o: $(SRCDIR)/%.c | $(BUILDDIR_OBJ)
	$(CC) $(CFLAGS) $< -o $@

# Link kernel
$(KERNEL): $(BOOTOBJ) $(COBJS) | $(BUILDDIR_BIN)
	$(LD) $(LDFLAGS) -o $@ $^

# Clean build artifacts
clean:
	rm -rf build

-include $(COBJS:.o=.d)

.PHONY: all clean

