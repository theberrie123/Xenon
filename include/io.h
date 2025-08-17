#ifndef IO_H
#define IO_H


#include "type.h"


uint8_t inb(uint16_t port);
void outb(uint16_t port, uint8_t val);

#endif
