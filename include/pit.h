#ifndef PIT_H
#define PIT_H


#include <xenon/type.h>



#define PIT_CHANNEL0_PORT       0x40
#define PIT_COMMAND_PORT        0x43
#define PIT_INPUT_FREQ          1193182u


void pit_init(uint32_t frequency_hz);
void pit_set_frequency(uint32_t frequency_hz);
uint64_t get_jiffies();
void pit_irq_handler();

#endif /* PIT_H */
