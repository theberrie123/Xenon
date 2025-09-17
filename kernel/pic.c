#include "pic.h"


void pic_remap(int offset1, int offset2)
{
        UINT8 a1 = inb(PIC1_DATA);
        UINT8 a2 = inb(PIC2_DATA);

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

void pic_set_mask(UINT8 mask1, UINT8 mask2)
{
        outb(0x21, mask1);
        outb(0xA1, mask2);
}

void pic_init()
{
        pic_remap(0x20, 0x28);
        pic_set_mask(0xFC, 0xFF);
}
