#ifndef INTERRUPTS_H
#define INTERRUPTS_H


#include <xenon/type.h>


void irq0_c();
void irq1_c();

int keyboard_get_char();


void enable_interrupts();
void disable_interrupts();


#endif /* INTERRUPTS_H */
