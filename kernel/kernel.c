#include "../include/type.h"
#include "../include/idt.h"
#include "../include/tty.h"
#include "../include/io.h"
#include "../include/gdt.h"
#include "../include/pic.h"
#include "../include/pit.h"
#include "../include/isr.h"
#include "../include/paging.h"


#define VGA_MEMORY (uint16_t*)0xB8000
#define VGA_WIDTH 80


void enable_interrupts()
{
        asm volatile ("sti");
}

void disable_interrupts()
{
        asm volatile ("cli");
}



void kmain()
{
        kinit();
        gdt_init();
        idt_init();
        isr_init();
        pic_init();
        pit_init(100);


        __asm__ volatile ("sti");

        while (1) { }
}
