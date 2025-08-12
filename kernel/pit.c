#include "../include/io.h"
#include "../include/pit.h"

#include "../include/type.h"

volatile uint64_t pit_ticks = 0;

void pit_irq_handler(void) {
    pit_ticks++;
    // Send EOI to PIC
    outb(0x20, 0x20);
}

uint32_t pit_get_ticks() {
    return pit_ticks;
}


void pit_init(uint32_t frequency) {
    uint32_t divisor = 1193180 / frequency;
    // Command port: channel 0, lobyte/hibyte, mode 3 (square wave)
    outb(0x43, 0x36);
    // Send low byte
    outb(0x40, divisor & 0xFF);
    // Send high byte
    outb(0x40, (divisor >> 8) & 0xFF);
}
