#ifndef PIC_H
#define PIC_H


#include "xenon/type.h"
#include "io.h"


#define PIC1_COMMAND 0x20
#define PIC1_DATA 0x21
#define PIC2_COMMAND 0xA0
#define PIC2_DATA 0xA1
#define PIC_EOI 0x20


void pic_init();
void pic_set_mask(uint8_t mask1, uint8_t mask2);
void pic_remap(int offset1, int offset2);


#endif /* PIC_H */
