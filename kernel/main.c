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
#include "xenon/memory.h"

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

void test_heap()
{
        int size1 = 32;
        int size2 = 64;
        void *a = kmalloc(size1);
        kprintf("Allocated '%d' for '%c'", size1, 'a');
        kprintf("%d", sizeof(a));
}

void kmain()
{
        init();

        kprintf("Kernel initialized...\n");
        
        test_heap();
        

        while (1) __asm__ __volatile__ ("hlt");
}
