#include "../include/io.h"


UINT8 inb(UINT16 port)
{
        UINT8 ret;
        __asm__ __volatile__ ("inb %1, %0" : "=a"(ret) : "Nd"(port));
        return ret;
}

void outb(UINT16 port, UINT8 val)
{
        __asm__ __volatile__ ("outb %0, %1" : : "a"(val), "Nd"(port));
}
