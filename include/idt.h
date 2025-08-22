#ifndef IDT_H
#define IDT_H


#include <xenon/type.h>


struct idt_entry {
    uint16_t isr_low; // the lower 16 bits of the ISR's address
    uint16_t kernel_cs; // the GDT segment selector that the CPU will load into CS before calling the ISR
    uint8_t reserved; // set the zero
    uint8_t  attributes; // type and attributes
    uint16_t isr_high; // the higher 16 bits of the ISR's address
} __attribute__((packed));

struct idtr {
        uint16_t limit;
        uint32_t base;
} __attribute__((packed));


void idt_init();
void idt_set_descriptor(uint8_t vector, void *isr, uint8_t flags);
void idt_set_entry(uint8_t vector, void *isr, uint8_t flags);


#endif /* IDT_H */
