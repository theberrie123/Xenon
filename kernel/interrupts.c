#include "interrupts.h"


void enable_interrupts()
{
        __asm__ __volatile__ ("sti");
}

void disable_interrupts()
{
        __asm__ __volatile__ ("cli");
}
