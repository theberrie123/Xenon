AS = as
ASFLAGS = -32

CC = gcc
CFLAGS = -m32 -ffreestanding -fno-stack-protector -fno-pic -fno-pie -Wall -Wextra -Iinclude -c

LD = ld
LDFLAGS = -m elf_i386 -T linker.ld

QEMU = qemu-system-x86_64

SRCDIR = kernel
BUILDDIR_OBJ = build/obj
BUILDDIR_BIN = build/bin
BUILDDIR_BOOT = build/boot
GRUBDIR = $(BUILDDIR_BOOT)/grub

ISO = $(BUILDDIR_BIN)/os.iso

CFILES := $(wildcard $(SRCDIR)/*.c)

COBJS := $(patsubst $(SRCDIR)/%.c,$(BUILDDIR_OBJ)/%.o,$(CFILES))

BOOTOBJ = $(BUILDDIR_OBJ)/boot.o

KERNEL = $(BUILDDIR_BIN)/kernel.elf

all: $(ISO)

$(BOOTOBJ): $(SRCDIR)/boot.s
	mkdir -p $(BUILDDIR_OBJ)
	$(AS) $(ASFLAGS) $< -o $@

$(BUILDDIR_OBJ)/%.o: $(SRCDIR)/%.c
	mkdir -p $(BUILDDIR_OBJ)
	$(CC) $(CFLAGS) $< -o $@

$(KERNEL): $(BOOTOBJ) $(COBJS)
	mkdir -p $(BUILDDIR_BIN)
	$(LD) $(LDFLAGS) -o $@ $^

$(GRUBDIR)/grub.cfg:
	mkdir -p $(GRUBDIR)
	echo 'set timeout=0' > $(GRUBDIR)/grub.cfg
	echo 'set default=0' >> $(GRUBDIR)/grub.cfg
	echo 'menuentry "Lithium" {' >> $(GRUBDIR)/grub.cfg
	echo '  multiboot /boot/kernel.elf' >> $(GRUBDIR)/grub.cfg
	echo '  boot' >> $(GRUBDIR)/grub.cfg
	echo '}' >> $(GRUBDIR)/grub.cfg

$(ISO): $(KERNEL) $(GRUBDIR)/grub.cfg
	cp $(KERNEL) $(BUILDDIR_BOOT)/
	grub-mkrescue -o $@ build

clean:
	rm -rf build/obj/*.o build/bin/kernel.elf build/bin/os.iso build/boot

run: $(ISO)
	$(QEMU) -cdrom $<

.PHONY: all clean run

