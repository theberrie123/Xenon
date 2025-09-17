#include "pit.h"


static __volatile__ UINT64 jiffies = 0;


void pit_tick()
{
        jiffies++;
}

void pit_set_frequency(UINT32 frequency_hz)
{
        if (frequency_hz == 0) {
                return;
        }

        UINT32 divisor = (PIT_INPUT_FREQ + frequency_hz/2) / frequency_hz;
        if (divisor == 0) {
                divisor = 1;
        }
        if (divisor > 0xFFFF) {
                divisor = 0xFFFF;
        }

        outb(PIT_COMMAND_PORT, 0x36); 
        UINT8 lo = divisor & 0xFF;
        UINT8 hi = (divisor >> 8) & 0xFF;
        outb(PIT_CHANNEL0_PORT, lo);
        outb(PIT_CHANNEL0_PORT, hi);
}

void pit_init()
{
        pit_set_frequency((UINT32)PIT_HZ);

}

UINT64 get_jiffies()
{
        UINT64 v = jiffies;
        return v;
}
