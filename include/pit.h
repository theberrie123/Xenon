#ifndef PIT_H
#define PIT_H


#include "xenon/type.h"
#include "io.h"
#include "tty.h"



#define PIT_CHANNEL0_PORT 0x40
#define PIT_COMMAND_PORT 0x43
#define PIT_INPUT_FREQ 1193182u
#define PIT_HZ 100


void pit_init();
void pit_set_frequency(UINT32 frequency_hz);
UINT64 get_jiffies();
void pit_irq_handler();


#endif /* PIT_H */
