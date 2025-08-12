#include "../include/interrupts.h"
#include "../include/io.h"
#include "../include/tty.h"


#define KEYBOARD_DATA_PORT 0x60
#define BUFFER_SIZE 128

static volatile uint8_t keyboard_buffer[BUFFER_SIZE];
static volatile uint32_t buffer_head = 0;
static volatile uint32_t buffer_tail = 0;

void isr_handler_c(uint32_t int_no, uint32_t err_code) {
    // For now just halt CPU on exceptions
    asm volatile ("cli; hlt");
}

void irq0_c(void) {
    // Timer IRQ placeholder
}

void keyboard_buffer_put(uint8_t scancode) {
    uint32_t next = (buffer_head + 1) % BUFFER_SIZE;
    if (next != buffer_tail) { // avoid overflow
        keyboard_buffer[buffer_head] = scancode;
        buffer_head = next;
    }
}

int keyboard_buffer_get(void) {
    if (buffer_head == buffer_tail) {
        return -1; // buffer empty
    }
    uint8_t scancode = keyboard_buffer[buffer_tail];
    buffer_tail = (buffer_tail + 1) % BUFFER_SIZE;
    return scancode;
}

static const char scancode_map[128] = {
    0, 27, '1', '2', '3', '4', '5', '6', '7', '8', // 0x00-0x09
    '9', '0', '-', '=', '\b', '\t',                // 0x0A-0x0F
    'q', 'w', 'e', 'r', 't', 'y', 'u', 'i', 'o', 'p', // 0x10-0x19
    '[', ']', '\n', 0,                             // 0x1A-0x1D (0 = no mapping)
    'a', 's', 'd', 'f', 'g', 'h', 'j', 'k', 'l', ';', // 0x1E-0x27
    '\'', '`', 0, '\\',                            // 0x28-0x2B
    'z', 'x', 'c', 'v', 'b', 'n', 'm', ',', '.', '/', // 0x2C-0x34
    0, '*', 0, ' ',                               // 0x35-0x38 (space is 0x39 but here example)
    // ... fill as needed
};

int keyboard_get_char(void) {
    int scancode = keyboard_buffer_get();
    if (scancode == -1) return -1;

    int released = scancode & 0x80;
    int keycode = scancode & 0x7F;

    if (released) {
        // Key release — ignore for now
        return -1;
    }

    if (keycode < 128) {
        return scancode_map[keycode];
    }
    return -1;
}

void keyboard_interrupt(void)
{
        uint8_t scancode = inb(0x60);
        keyboard_buffer_put(scancode);
}
