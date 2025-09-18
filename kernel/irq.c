#include "irq.h"
#include "pit.h"


/* PIT tick */
__attribute__((naked))
void irq0_handler()
{
        __asm__ __volatile__ (
                "cli\n\t"
                "pusha\n\t"
                "call pit_tick\n\t"
                "popa\n\t"
                "mov $0x20, %al\n\t"
                "outb %al, $0x20\n\t"
                "sti\n\t"
                "iret\n\t"
        );
}
