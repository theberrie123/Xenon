#include "tty.h"
#include "gdt.h"
#include "idt.h"
#include "isr.h"
#include "pic.h"
#include "pit.h"
#include "interrupts.h"
#include "paging.h"
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
}



void kmain()
{
        init();


        kprintf("Kernel booted...\n");

        while (1) { }
}
