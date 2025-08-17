#include "../include/type.h"
#include "../include/idt.h"
#include "../include/tty.h"
#include "../include/io.h"
#include "../include/gdt.h"
#include "../include/pic.h"
#include "../include/irq.h"
#include "../include/interrupts.h"
#include "../include/fs.h"


#define INPUT_BUFFER_SIZE 256
static char input_buffer[INPUT_BUFFER_SIZE];
static int input_pos = 0;



void pic_set_mask(uint8_t mask1, uint8_t mask2) {
    outb(0x21, mask1); // Master PIC data port
    outb(0xA1, mask2); // Slave PIC data port
}


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
        gdt_init();
        idt_init();
        pic_remap(0x20, 0x28);
        pic_set_mask(0xFD, 0xFF);

        __asm__ volatile ("sti");

        kinit();
        kprintf("Loaded kernel...\n");

        init_root_fs();
        kprintf("Filesystem initialized...\n");

        list_dir(root_inode);

        while (1) { }
}
