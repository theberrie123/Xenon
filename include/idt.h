#ifndef IDT_H
#define IDT_H


#include "xenon/type.h"


struct idt_entry {
        UINT16 isr_low;
        UINT16 kernel_cs;
        UINT8 reserved;     
        UINT8 attributes; 
        UINT16 isr_high;
} __attribute__((packed));

struct idtr {
        UINT16 limit;
        UINT32 base;
} __attribute__((packed));


void idt_init();
void idt_set_descriptor(UINT8 vector, void *isr, UINT8 flags);
void idt_set_entry(UINT8 vector, void *isr, UINT8 flags);


#endif /* IDT_H */
