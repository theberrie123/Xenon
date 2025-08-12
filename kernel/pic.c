#include "../include/pic.h"
#include "../include/io.h"


void pic_remap(int offset1, int offset2)
{
        uint8_t a1, a2;

        a1 = inb(PIC1_DATA);
        a2 = inb(PIC2_DATA);

        outb(PIC1_COMMAND, 0x11);
        outb(PIC2_COMMAND, 0x11);

        outb(PIC1_DATA, offset1);
        outb(PIC2_DATA, offset2);

        outb(PIC1_DATA, 4);
        outb(PIC2_DATA, 2);

        outb(PIC1_DATA, 0x01);
        outb(PIC2_DATA, 0x01);

        outb(PIC1_DATA, a1);
        outb(PIC2_DATA, a2);
}
