#include "kernel.h"



void init()
{
        kinit();
        kprintf("[%%g  OK  %%w] initialized tty\n");
        gdt_init();
        kprintf("[%%g  OK  %%w] initialized gdt\n");
        idt_init();
        kprintf("[%%g  OK  %%w] initialized idt\n");
        isr_init();
        kprintf("[%%g  OK  %%w] initialized isr\n");
        pic_init();
        kprintf("[%%g  OK  %%w] initialized pic\n");
        pit_init(100);
        kprintf("[%%g  OK  %%w] initialized pit\n");
        enable_interrupts();
        kprintf("[%%g  OK  %%w] enabled interrupts\n");
        paging_init_identity_4mb();
        kprintf("[%%g  OK  %%w] initialized paging\n");
        scheduler_init();
        kprintf("[%%g  OK  %%w] initialized scheduler\n");
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
                                kprintf("[%%g  OK  %%w] initialized initramfs\n");
                                break;
                        }
                }
        } else {
                panic("initramfs module not found\n");
        }

        for (;;) __asm__ __volatile__ ("hlt");
}
