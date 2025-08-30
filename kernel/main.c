// kmain.c
#include "tty.h"
#include "gdt.h"
#include "idt.h"
#include "isr.h"
#include "pic.h"
#include "pit.h"
#include "interrupts.h"
#include "paging.h"
#include "kernel.h"
#include "multiboot.h"
#include "xenon/string.h"
#include "xenon/memory.h"


struct cpio_newc_header {
    char c_magic[6];
    char c_ino[8];
    char c_mode[8];
    char c_uid[8];
    char c_gid[8];
    char c_nlink[8];
    char c_mtime[8];
    char c_filesize[8];
    char c_devmajor[8];
    char c_devminor[8];
    char c_rdevmajor[8];
    char c_rdevminor[8];
    char c_namesize[8];
    char c_check[8];
};

// Align to 4 bytes
static unsigned long align4(unsigned long x) {
    return (x + 3) & ~3;
}


unsigned char *initramfs_start = 0;
unsigned long initramfs_size = 0;


void init()
{
        kinit();
        gdt_init();
        idt_init();
        isr_init();
        pic_init();
        pit_init(100);
        enable_interrupts();
        paging_init_identity_4mb();
}


static unsigned long hex_to_ulong(const char *hex, int len)
{
        unsigned long result = 0;
        for (int i = 0; i < len; i++) {
                char c = hex[i];
                result <<= 4;
                if (c >= '0' && c <= '9') result += c - '0';
                else if (c >= 'A' && c <= 'F') result += c - 'A' + 10;
                else if (c >= 'a' && c <= 'f') result += c - 'a' + 10;
        }
        return result;
}

void dump_initramfs_header() {
    if (!initramfs_start) {
        kprintf("No initramfs loaded!\n");
        return;
    }

    kprintf("First 32 bytes of initramfs:\n");
    for (int i = 0; i < 32; i++) {
        kprintf("%02x ", initramfs_start[i]);
    }
    kprintf("\n");
}


void parse_initramfs_root_simple() {
    unsigned char *ptr = initramfs_start;

    while (1) {
        // check magic (first 6 bytes)
        if (ptr[0] != '0' || ptr[1] != '7' || ptr[2] != '0' ||
            ptr[3] != '7' || ptr[4] != '0' || ptr[5] != '1') {
            kprintf("Invalid magic, stopping\n");
            break;
        }

        unsigned long namesize = hex_to_ulong((char *)ptr + 94, 8); // c_namesize at offset 94
        unsigned long filesize = hex_to_ulong((char *)ptr + 54, 8); // c_filesize at offset 54

        char *name = (char *)(ptr + sizeof(struct cpio_newc_header));

        // skip TRAILER!!! and "."
        if (namesize >= 11 && name[0] == 'T') break;
        if (namesize == 2 && name[0] == '.') { ptr += align4(sizeof(struct cpio_newc_header) + namesize + filesize); continue; }

        for (unsigned long i = 0; i < namesize - 1; i++) kprintf("%c", name[i]);
        kprintf("\n");

        ptr += align4(sizeof(struct cpio_newc_header) + namesize + filesize);
    }
}




void kmain(unsigned long magic, struct multiboot_info *mbi)
{
        init();

        kprintf("Kernel initialized...\n");
        
        if (magic != 0x2BADB002) {
                kprintf("Not booted with GRUB");
        }

        if (mbi->flags & MULTIBOOT_INFO_MODS) {
                unsigned int mods_count = mbi->mods_count;

                struct multiboot_module *mods = (struct multiboot_module *) mbi->mods_addr;
                
                for (unsigned int i = 0; i < mods_count; i++) {
                        struct multiboot_module *mod = &mods[i];

                        initramfs_start = (unsigned char *) mod->mod_start;
                        initramfs_size = mod->mod_end - mod->mod_start;
                        
                        kprintf("initramfs found\n");
                        kprintf("initramfs start: %x, size: %u\n", (unsigned int)initramfs_start, (unsigned int)initramfs_size);

                        if (initramfs_start) {
                                kprintf("Parsing initramfs root directory:\n");
                                parse_initramfs_root_simple();
                        }
                }
        }

        while (1) __asm__ __volatile__ ("hlt");
}
