#ifndef INTERRUPTS_H
#define INTERRUPTS_H

#include "type.h"


void isr_handler_c();
void irq0_c(void);
void irq1_c(void);

int keyboard_get_char(void);



#endif
