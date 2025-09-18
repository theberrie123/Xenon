#ifndef ISR_H
#define ISR_H


#include "xenon/type.h"
#include "kernel.h"


struct regs {
        uint32_t edi;
        uint32_t esi;
        uint32_t ebp;
        uint32_t esp;
        uint32_t ebx;
        uint32_t edx;
        uint32_t ecx;
        uint32_t eax;
        uint32_t int_no;
        uint32_t err_code;
};

typedef void (*isr_handler_t)(struct regs *);

void isr_handler_c(struct regs *regs);
void isr_init();
void isr_register_handler(uint8_t n, isr_handler_t handler);

#endif
