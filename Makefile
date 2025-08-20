AS = as
ASFLAGS = -32

CC = gcc
CFLAGS = -m32 -ffreestanding -fno-stack-protector -fno-pic -fno-pie \
         -Wall -Wextra -Iinclude -Iarch/x86/include -c -MMD -MP

LD = ld
LDFLAGS = -m elf_i386 -T linker.ld

QEMU = qemu-system-x86_64

SRCDIR = kernel
ARCHDIR = arch
BUILDDIR_OBJ = build/obj
BUILDDIR_BIN = build/bin
BUILDDIR_BOOT = build/boot
GRUBDIR = $(BUILDDIR_BOOT)/grub

ISO = $(BUILDDIR_BIN)/os.iso

CFILES := $(wildcard $(SRCDIR)/*.c)
COBJS := $(patsubst $(SRCDIR)/%.c,$(BUILDDIR_OBJ)/%.o,$(CFILES))
BOOTOBJ = $(BUILDDIR_OBJ)/boot.o
KERNEL = $(BUILDDIR_BIN)/kernel.elf

DIRS = $(BUILDDIR_OBJ) $(BUILDDIR_BIN) $(BUILDDIR_BOOT) $(GRUBDIR)

all: $(ISO)

$(DIRS):
	@mkdir -p $@

$(BOOTOBJ): $(ARCHDIR)/x86/boot/boot.s | $(BUILDDIR_OBJ)
	$(AS) $(ASFLAGS) $< -o $@

$(BUILDDIR_OBJ)/%.o: $(SRCDIR)/%.c | $(BUILDDIR_OBJ)
	$(CC) $(CFLAGS) $< -o $@

$(KERNEL): $(BOOTOBJ) $(COBJS) | $(BUILDDIR_BIN)
	$(LD) $(LDFLAGS) -o $@ $^

$(GRUBDIR)/grub.cfg: | $(GRUBDIR)
	@test -f $@ || { \
	 echo 'set timeout=0' > $@; \
	 echo 'set default=0' >> $@; \
	 echo 'menuentry "Xenon" {' >> $@; \
	 echo '  multiboot /boot/kernel.elf' >> $@; \
	 echo '  boot' >> $@; \
	 echo '}' >> $@; }

$(ISO): $(KERNEL) $(GRUBDIR)/grub.cfg | $(BUILDDIR_BIN)
	cp $(KERNEL) $(BUILDDIR_BOOT)/
	grub-mkrescue -o $@ build

clean:
	rm -rf build

build: $(ISO)
	$(MAKE) -j$$(nproc)
	$(QEMU) -cdrom $(ISO)

-include $(COBJS:.o=.d)

.PHONY: all clean build
