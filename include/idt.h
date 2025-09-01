#ifndef IDT_H
#define IDT_H


#include "xenon/type.h"


struct idt_entry {
        uint16_t isr_low;
        uint16_t kernel_cs;
        uint8_t reserved;     
        uint8_t  attributes; 
        uint16_t isr_high;
} __attribute__((packed));

struct idtr {
        uint16_t limit;
        uint32_t base;
} __attribute__((packed));


void idt_init();
void idt_set_descriptor(uint8_t vector, void *isr, uint8_t flags);
void idt_set_entry(uint8_t vector, void *isr, uint8_t flags);


#endif /* IDT_H */
