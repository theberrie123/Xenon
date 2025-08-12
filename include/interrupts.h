#ifndef INTERRUPTS_H
#define INTERRUPTS_H

#include "type.h"


void isr_handler_c(uint32_t int_no, uint32_t err_code);
void irq0_c(void);
void irq1_c(void);

int keyboard_get_char(void);



#endif
