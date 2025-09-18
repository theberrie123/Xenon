#include "pit.h"


static __volatile__ uint64_t jiffies = 0;


void pit_tick()
{
        jiffies++;
}

void pit_set_frequency(uint32_t frequency_hz)
{
        if (frequency_hz == 0) {
                return;
        }

        uint32_t divisor = (PIT_INPUT_FREQ + frequency_hz/2) / frequency_hz;
        if (divisor == 0) {
                divisor = 1;
        }
        if (divisor > 0xFFFF) {
                divisor = 0xFFFF;
        }

        outb(PIT_COMMAND_PORT, 0x36); 
        uint8_t lo = divisor & 0xFF;
        uint8_t hi = (divisor >> 8) & 0xFF;
        outb(PIT_CHANNEL0_PORT, lo);
        outb(PIT_CHANNEL0_PORT, hi);
}

void pit_init()
{
        pit_set_frequency((uint32_t)PIT_HZ);

}

uint64_t get_jiffies()
{
        return (uint64_t)jiffies;
}
