#include "kernel.h"
#include "syscall.h"


void init_main()
{
        kprintf("init (PID 1) starting...\n");
        for (;;) {

        }
}

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
        pit_init();
        kprintf("[%%g  OK  %%w] initialized pit\n");
        enable_interrupts();
        kprintf("[%%g  OK  %%w] enabled interrupts\n");
        paging_init_identity_4mb();
        kprintf("[%%g  OK  %%w] initialized paging\n");

}

void write(int fd, const void *buf, size_t count)
{
        register int write_register_fd __asm__ ("ebx") = fd;
        register const void *write_register_buf __asm__ ("ecx") = buf;
        register size_t write_register_count __asm__ ("edx") = count;

        __asm__ __volatile__ (
                "movl $1, %%eax\n\t"
                "int $0x80"
                :
                : "b"(write_register_fd), "c"(write_register_buf), "d"(write_register_count)
                : "eax"
        );
}


void kmain(size_t magic, struct multiboot_info *mbi)
{
        init();

        if (magic != 0x2BADB002) {
                panic("not booted with GRUB\n");
        }

        if (mbi->flags & MULTIBOOT_INFO_MODS) {
                struct multiboot_module *mods = (struct multiboot_module *) mbi->mods_addr;
                for (uint32_t i = 0; i < mbi->mods_count; i++) {
                        struct multiboot_module *mod = &mods[i];
                        if (mod->mod_start && mod->mod_end) {
                                uint8_t *initramfs_start = (uint8_t *) mod->mod_start;
                                uint8_t *initramfs_end = (uint8_t *) mod->mod_end;
                                size_t initramfs_size = mod->mod_end - mod->mod_start;

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

        print_dir("/");



        for (;;) __asm__ __volatile__ ("hlt");
}
