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
COBJS 		:= $(patsubst $(SRCDIR)/%.c,$(BUILDDIR)/%.o,$(CFILES))
BOOTOBJ 	= $(BUILDDIR)/boot.o

ISO_NAME        = xenon.iso
ISO_DIR         = iso
INITRAMFS_DIR   = initramfs
INITRAMFS_IMG   = $(ISO_DIR)/boot/initramfs.cpio

GRUB_CFG        = $(ISO_DIR)/boot/grub/grub.cfg
QEMU_OPTS       = -cdrom $(ISO_DIR)/$(ISO_NAME) -m 512M

.PHONY: all kernel iso run clean

all: iso

# --- Kernel Build ---

$(BOOTOBJ): $(ARCHDIR)/x86/boot/boot.s
	@mkdir -p $(dir $@)
	$(AS) $(ASFLAGS) $< -o $@

$(BUILDDIR)/%.o: $(SRCDIR)/%.c
	@mkdir -p $(dir $@)
	$(CC) $(CFLAGS) $< -o $@

$(KERNEL): $(BOOTOBJ) $(COBJS)
	$(LD) $(LDFLAGS) -o $(BUILDDIR)/$@ $(BOOTOBJ) $(COBJS)
	mv $(BUILDDIR)/$@ ./

# --- Initramfs ---

$(INITRAMFS_IMG):
	@mkdir -p $(dir $@)
	cd $(INITRAMFS_DIR) && find . | cpio -H newc -o > ../$@

# --- ISO Creation ---

$(ISO_DIR)/boot:
	mkdir -p $(ISO_DIR)/boot/grub

iso: $(KERNEL) $(ISO_DIR)/boot $(INITRAMFS_IMG)
	rm -f $(ISO_DIR)/$(ISO_NAME)   # <---- remove old ISO before remaking
	cp $(KERNEL) $(ISO_DIR)/boot/
	echo 'set timeout=5' > $(GRUB_CFG)
	echo 'set default=0' >> $(GRUB_CFG)
	echo 'menuentry "Xenon" {' >> $(GRUB_CFG)
	echo '    multiboot /boot/$(KERNEL)' >> $(GRUB_CFG)
	echo '    module /boot/initramfs.cpio' >> $(GRUB_CFG)
	echo '}' >> $(GRUB_CFG)
	grub-mkrescue -o $(ISO_DIR)/$(ISO_NAME) $(ISO_DIR)
# --- Run with QEMU ---

run: iso
	qemu-system-i386 $(QEMU_OPTS)

# --- Cleanup ---

clean:
	rm -rf $(BUILDDIR) $(KERNEL) $(ISO_DIR)

-include $(COBJS:.o=.d)

