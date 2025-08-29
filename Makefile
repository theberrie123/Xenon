# -----------------------------
# Toolchain
# -----------------------------
AS      = as
ASFLAGS = -32

CC      = gcc
CFLAGS  = -m32 -ffreestanding -fno-stack-protector -fno-pic -fno-pie \
          -Wall -Wextra -Iinclude -Iarch/x86/include -c -MMD -MP

LD      = ld
LDFLAGS = -m elf_i386 -T linker.ld

# -----------------------------
# Directories
# -----------------------------
SRCDIR    = kernel
ARCHDIR   = arch
BUILDDIR  = build

KERNEL    = vmxez

# -----------------------------
# Source files
# -----------------------------
CFILES := $(shell find $(SRCDIR) -name '*.c')
COBJS  := $(patsubst $(SRCDIR)/%.c,$(BUILDDIR)/%.o,$(CFILES))

BOOTOBJ = $(BUILDDIR)/boot.o

# -----------------------------
# Targets
# -----------------------------
all: $(KERNEL)

# Assemble boot sector
$(BOOTOBJ): $(ARCHDIR)/x86/boot/boot.s
	@mkdir -p $(dir $@)
	$(AS) $(ASFLAGS) $< -o $@

# Compile C files (handle subdirectories automatically)
$(BUILDDIR)/%.o: $(SRCDIR)/%.c
	@mkdir -p $(dir $@)   # ensure directory exists for both .o and .d
	$(CC) $(CFLAGS) $< -o $@

# Link kernel
$(KERNEL): $(BOOTOBJ) $(COBJS)
	$(LD) $(LDFLAGS) -o $(BUILDDIR)/$@ $(BOOTOBJ) $(COBJS)
	mv $(BUILDDIR)/$@ ./   # move final binary to main folder

# Clean build artifacts
clean:
	rm -rf $(BUILDDIR) $(KERNEL)

# Include dependency files
-include $(COBJS:.o=.d)

.PHONY: all clean

