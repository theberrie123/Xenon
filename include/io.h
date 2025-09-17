#ifndef IO_H
#define IO_H


#include "xenon/type.h"


UINT8 inb(UINT16 port);
void outb(UINT16 port, UINT8 val);

#endif /* IO_H */
