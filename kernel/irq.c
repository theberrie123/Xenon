#include "irq.h"
#include "sched/task.h"
#include "pit.h"

__attribute__((naked))
void irq0_handler()
{
    __asm__ __volatile__ (
        "cli\n\t"
        "pusha\n\t"          // save all GPRs
        "call pit_tick\n\t"
        "popa\n\t"           // restore GPRs
        "mov $0x20, %al\n\t"
        "outb %al, $0x20\n\t" // send EOI to PIC
        "sti\n\t"
        "iret\n\t"
    );
}
