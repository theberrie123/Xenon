#ifndef MULTIBOOT_H
#define MULTIBOOT_H


#include "xenon/type.h"


#define MULTIBOOT_HEADER_MAGIC  0x1BADB002
#define MULTIBOOT_HEADER_FLAGS  0x00010003

#define MULTIBOOT_INFO_MEMORY         0x00000001
#define MULTIBOOT_INFO_BOOTDEV        0x00000002
#define MULTIBOOT_INFO_CMDLINE        0x00000004
#define MULTIBOOT_INFO_MODS           0x00000008
#define MULTIBOOT_INFO_AOUT_SYMS      0x00000010
#define MULTIBOOT_INFO_ELF_SHDR       0x00000020
#define MULTIBOOT_INFO_MEM_MAP        0x00000040
#define MULTIBOOT_INFO_DRIVE_INFO     0x00000080
#define MULTIBOOT_INFO_CONFIG_TABLE   0x00000100
#define MULTIBOOT_INFO_BOOT_LOADER_NAME 0x00000200
#define MULTIBOOT_INFO_APM_TABLE      0x00000400
#define MULTIBOOT_INFO_VBE_INFO       0x00000800
#define MULTIBOOT_INFO_FRAMEBUFFER     0x00001000

struct multiboot_module {
        UINT32 mod_start;
        UINT32 mod_end;
        UINT32 string;
        UINT32 reserved;
} __attribute__((packed));

struct multiboot_info {
        UINT32 flags;
        UINT32 mem_lower;
        UINT32 mem_upper;
        UINT32 boot_device;
        UINT32 cmdline;
        UINT32 mods_count;
        UINT32 mods_addr;
        UINT32 syms[4];
        UINT32 mmap_length;
        UINT32 mmap_addr;
        UINT32 drives_length;
        UINT32 drives_addr;
        UINT32 config_table;
        UINT32 boot_loader_name;
        UINT32 apm_table;
        UINT32 vbe_control_info;
        UINT32 vbe_mode_info;
        UINT32 vbe_mode;
        UINT32 vbe_interface_seg;
        UINT32 vbe_interface_off;
        UINT32 vbe_interface_len;
} __attribute__((packed));

struct multiboot_header {
        UINT32 magic;
        UINT32 flags;
        UINT32 checksum;
        UINT32 header_addr;
        UINT32 load_addr;
        UINT32 bss_end_addr;
        UINT32 entry_addr;
} __attribute__((packed));


extern struct multiboot_info *mbi;

#endif /* MULTIBOOT_H */
