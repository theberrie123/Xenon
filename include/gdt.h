#ifndef GDT_H
#define GDT_H


#include "xenon/type.h"


struct gdt_entry {
        UINT16 limit_low;
        UINT16 base_low;
        UINT8 base_middle;
        UINT8 access;
        UINT8 granularity;
        UINT8 base_high;
} __attribute__((packed));

struct gdt_ptr {
    UINT16 limit;
    UINT32 base;
} __attribute__((packed));


void gdt_init();


#endif /* GDT_H */
