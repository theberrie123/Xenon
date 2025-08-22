#include "../include/irq.h"

volatile uint64_t jiffies = 0;

void pit_tick()
{
    jiffies++;
    // scheduler/timer code here
}

__attribute__((naked))
void irq0_handler()
{
        __asm__ __volatile__ (
                "cli\n\t"               // clear interrupts
                "push %eax\n\t"
                "push %ebx\n\t"
                "push %ecx\n\t"
                "push %edx\n\t"
                "push %esi\n\t"
                "push %edi\n\t"
                "call pit_tick\n\t"
                "pop %edi\n\t"
                "pop %esi\n\t"
                "pop %edx\n\t"
                "pop %ecx\n\t"
                "pop %ebx\n\t"
                "pop %eax\n\t"
                "mov $0x20, %al\n\t"    // EOI
                "outb %al, $0x20\n\t"
                "sti\n\t"
                "iret\n\t"
        );
}


void irq1_handler() __attribute__((naked));
void irq1_handler()
{
        __asm__ __volatile__ (
                "pusha\n\t"
                "call keyboard_interrupt\n\t"  // Your keyboard handler in C
                "movb $0x20, %al\n\t"
                "outb %al, $0x20\n\t"  // Send EOI to master PIC
                "popa\n\t"
                "iret\n\t"
        );
}
