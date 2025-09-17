#ifndef ISR_H
#define ISR_H


#include "xenon/type.h"
#include "kernel.h"


struct regs {
        UINT32 edi;
        UINT32 esi;
        UINT32 ebp;
        UINT32 esp;
        UINT32 ebx;
        UINT32 edx;
        UINT32 ecx;
        UINT32 eax;
        UINT32 int_no;
        UINT32 err_code;
};

typedef void (*isr_handler_t)(struct regs *);

void isr_handler_c(struct regs *regs);
void isr_init();
void isr_register_handler(UINT8 n, isr_handler_t handler);

#endif /* ISR_H */
