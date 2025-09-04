#include "kernel.h"



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
        scheduler_init();    
}



void kmain(unsigned long magic, struct multiboot_info *mbi)
{
        init();

        if (magic != 0x2BADB002) {
                panic("not booted with GRUB\n");
        }

        if (mbi->flags & MULTIBOOT_INFO_MODS) {
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

                                initramfs_init(initramfs);
                                break;
                        }
                }
        } else {
                panic("initramfs module not found\n");
        }

        for (;;) __asm__ __volatile__ ("hlt");
}
