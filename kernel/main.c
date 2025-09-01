#include "tty.h"
#include "gdt.h"
#include "idt.h"
#include "pic.h"
#include "pit.h"
#include "interrupts.h"
#include "kernel.h"
#include "multiboot.h"
#include "xenon/string.h"
#include "xenon/memory.h"
#include "init/initramfs.h"
#include "isr.h"
#include "paging.h"


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

void kmain(unsigned long magic, struct multiboot_info *mbi)
{
    init();

    if (magic != 0x2BADB002) {
        kprintf("[  %%rERR%%w ]  not booted with GRUB\n");
        return;
    }

    /* Unpack initramfs if present (you already do this earlier in your code).
       If unpack_to_rootfs has been called above, find_file_in_initramfs can locate files. */
    if (mbi->flags & MULTIBOOT_INFO_MODS) {
        /* your existing scanning/unpack code here (unchanged) */
        struct multiboot_module *mods = (struct multiboot_module *) mbi->mods_addr;
        for (unsigned int i = 0; i < mbi->mods_count; i++) {
            struct multiboot_module *mod = &mods[i];
            if (mod->mod_start && mod->mod_end) {
                unsigned char *initramfs_start = (unsigned char *) mod->mod_start;
                unsigned char *initramfs_end = (unsigned char *) mod->mod_end;
                unsigned long initramfs_size = mod->mod_end - mod->mod_start;

                struct initramfs initramfs = {
                    .start = initramfs_start,
                    .end = initramfs_end,
                    .size = initramfs_size,
                };

                kprintf("[  %%gOK%%w  ]  initramfs module found\n");

                unpack_to_rootfs(initramfs); /* your function copies and mounts it */
                break;
            }
        }
    } else {
        kprintf("[  %%rERR%%w ]  initramfs module not found\n");
    }

        struct file_in_ram init_file = find_file_in_initramfs("/init");
        if (init_file.data == NULL) {
                kprintf("[  %%rERR%%w ]  /init not found\n");
        } else {
                kprintf(" /init found, size=%u", init_file.size);
        }

    for (;;) __asm__ __volatile__ ("hlt");
}

