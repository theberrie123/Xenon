#include "../include/io.h"
#include "../include/pit.h"
#include "../include/tty.h"


/* number of PIT ticks since boot */
static __volatile__ uint64_t jiffies = 0;

static uint32_t pit_hz = 100; /* default */

void pit_set_frequency(uint32_t frequency_hz)
{
        if (frequency_hz == 0) {
                return;
        }

        /* divisor calc with rounding: (input + f/2)/f */
        uint32_t divisor = (PIT_INPUT_FREQ + frequency_hz/2) / frequency_hz;
        if (divisor == 0) {
                divisor = 1;
        }
        if (divisor > 0xFFFF) {
                divisor = 0xFFFF;
        }

        /* command byte:
        *       00 11 011 0
        *       bits 7-6: select channel (00 = channel 0)
        *       bits 5-4: access mode (11 = lobyte/hibyte)
        *       bits 3-1: mode (011 = mode 3 square wave)
        *       bit 0: BCD (0 = 16-bit binary)
        */
        outb(PIT_COMMAND_PORT, 0x36); /* 0x36 = 00 11 011 0 */
        uint8_t lo = divisor & 0xFE;
        uint8_t hi = (divisor >> 8) & 0xFF;
        outb(PIT_CHANNEL0_PORT, lo);
        outb(PIT_CHANNEL0_PORT, hi);

        pit_hz = frequency_hz;
}

void pit_init(uint32_t frequency_hz)
{
        pit_set_frequency(frequency_hz);

        kprintf("[  %%gOK%%w  ]  Initialized PIT with %d Hz\n", frequency_hz);

}

uint64_t get_jiffies()
{
        uint64_t v;
        v = jiffies;
        return v;
}
