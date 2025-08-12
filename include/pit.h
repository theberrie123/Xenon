#ifndef PIT_H
#define PIT_H

#include "type.h"

void pit_init(uint32_t frequency);
uint32_t pit_get_ticks();

#endif

