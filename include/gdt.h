#ifndef GDT_H
#define GDT_H


#include "xenon/type.h"

struct gdt_entry {
    uint16_t limit_low;     // lower 16 bits of limit
    uint16_t base_low;      // lower 16 bits of base address
    uint8_t  base_middle;   // bits 16-23 of base address
    uint8_t  access;        // access flags
    uint8_t  granularity;   // granularity and limit bits 16-19
    uint8_t  base_high;     // bits 24-31 of base address
} __attribute__((packed));

struct gdt_ptr {
    uint16_t limit;         // size of GDT -1
    uint32_t base;          // address of GDT
} __attribute__((packed));

void gdt_init();

#endif /* GDT_H */
