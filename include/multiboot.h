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
        uint32_t mod_start;
        uint32_t mod_end;
        uint32_t string;
        uint32_t reserved;
} __attribute__((packed));

struct multiboot_info {
        uint32_t flags;
        uint32_t mem_lower;
        uint32_t mem_upper;
        uint32_t boot_device;
        uint32_t cmdline;
        uint32_t mods_count;
        uint32_t mods_addr;
        uint32_t syms[4];
        uint32_t mmap_length;
        uint32_t mmap_addr;
        uint32_t drives_length;
        uint32_t drives_addr;
        uint32_t config_table;
        uint32_t boot_loader_name;
        uint32_t apm_table;
        uint32_t vbe_control_info;
        uint32_t vbe_mode_info;
        uint32_t vbe_mode;
        uint32_t vbe_interface_seg;
        uint32_t vbe_interface_off;
        uint32_t vbe_interface_len;
} __attribute__((packed));

struct multiboot_header {
        uint32_t magic;
        uint32_t flags;
        uint32_t checksum;
        uint32_t header_addr;
        uint32_t load_addr;
        uint32_t bss_end_addr;
        uint32_t entry_addr;
} __attribute__((packed));


extern struct multiboot_info *mbi;

#endif /* MULTIBOOT_H */
