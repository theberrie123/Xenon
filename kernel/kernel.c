#include "../include/type.h"
#include "../include/idt.h"
#include "../include/tty.h"
#include "../include/io.h"
#include "../include/gdt.h"
#include "../include/pic.h"
#include "../include/irq.h"
#include "../include/interrupts.h"


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
        kprintf("Xenon Lithium 1.0.0 (tty)\n\n");
        kprintf("root@xenoniso ~ # ");

    while (1) {
        int c = keyboard_get_char();
        if (c != -1) {
            if (c == '\r' || c == '\n') {
                input_buffer[input_pos] = '\0'; // terminate string
                kprintf("\nYou typed: %s\n", input_buffer);
                input_pos = 0;
                kprintf("root@xenoniso ~ # ");
            } else if (c == '\b') {
                if (input_pos > 0) {
                    input_pos--;
                    kputchar('\b');
                }
            } else if (input_pos < INPUT_BUFFER_SIZE - 1) {
                input_buffer[input_pos++] = (char)c;
                kputchar((char)c);
            }
        }
    }
}
