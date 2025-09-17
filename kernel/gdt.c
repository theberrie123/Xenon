#include "gdt.h"
#include "tty.h"

static struct gdt_entry gdt[3];
static struct gdt_ptr gdtr;

static void gdt_set_entry(int idx, UINT32 base, UINT32 limit, UINT8 access, UINT8 gran)
{
        gdt[idx].limit_low = limit & 0xFFFF;
        gdt[idx].base_low = base & 0xFFFF;
        gdt[idx].base_middle = (base >> 16) & 0xFF;
        gdt[idx].access = access;
        gdt[idx].granularity = ((limit >> 16) & 0x0F) | (gran & 0xF0);
        gdt[idx].base_high = (base >> 24) & 0xFF;
}

void gdt_init()
{
        gdt_set_entry(0, 0, 0, 0, 0);
        gdt_set_entry(1, 0, 0xFFFFF, 0x9A, 0xCF);
        gdt_set_entry(2, 0, 0xFFFFF, 0x92, 0xCF);

        gdtr.limit = sizeof(gdt) - 1;
        gdtr.base = (UINT32)&gdt;

        __asm__ __volatile__ ("lgdt %0" : : "m"(gdtr));

        __asm__ __volatile__ (
                "mov $0x10, %%ax\n\t"
                "mov %%ax, %%ds\n\t"
                "mov %%ax, %%es\n\t"
                "mov %%ax, %%fs\n\t"
                "mov %%ax, %%gs\n\t"
                "mov %%ax, %%ss\n\t"
                "jmp $0x08, $.flush\n\t"
                ".flush:\n\t"
                : : : "ax"
        );

}
