#ifndef ISR_H
#define ISR_H


#include <xenon/type.h>


typedef struct registers {
    uint32_t ds;
    uint32_t edi, esi, ebp, esp, ebx, edx, ecx, eax;
    uint32_t int_no, err_code;
    uint32_t eip, cs, eflags, useresp, ss;
} registers_t;

typedef void (*isr_handler_t)(registers_t*);

void isr_init();
void isr_register_handler(uint8_t n, isr_handler_t handler);
void isr_handler_c(registers_t *regs);

#endif /* ISR_H */
