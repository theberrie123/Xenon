#include "../include/idt.h"
#include "../include/type.h"
#include "../include/irq.h"
#include "../include/tty.h"


#define IDT_SIZE 256

static struct idt_entry idt[IDT_SIZE];
static struct idtr idtr;

/* Forward declarations of ISR stubs */
#define ISR_NO_ERR_STUB(n)                    \
void isr_stub_##n(void) __attribute__((naked)); \
void isr_stub_##n(void) {                      \
        __asm__ __volatile__ (                             \
                "pushl $0\n\t"                        \
                "pushl $" #n "\n\t"                   \
                "jmp isr_handler_c\n\t"               \
        );                                        \
}

#define ISR_ERR_STUB(n)                       \
void isr_stub_##n(void) __attribute__((naked)); \
void isr_stub_##n(void) {                      \
        __asm__ __volatile__ (                             \
                "pushl $" #n "\n\t"                    \
                "jmp isr_handler_c\n\t"               \
        );                                        \
}

/* Define the 32 CPU exception stubs */
ISR_NO_ERR_STUB(0)
ISR_NO_ERR_STUB(1)
ISR_NO_ERR_STUB(2)
ISR_NO_ERR_STUB(3)
ISR_NO_ERR_STUB(4)
ISR_NO_ERR_STUB(5)
ISR_NO_ERR_STUB(6)
ISR_NO_ERR_STUB(7)
ISR_ERR_STUB(8)
ISR_NO_ERR_STUB(9)
ISR_ERR_STUB(10)
ISR_ERR_STUB(11)
ISR_ERR_STUB(12)
ISR_ERR_STUB(13)
ISR_ERR_STUB(14)
ISR_NO_ERR_STUB(15)
ISR_NO_ERR_STUB(16)
ISR_ERR_STUB(17)
ISR_NO_ERR_STUB(18)
ISR_NO_ERR_STUB(19)
ISR_NO_ERR_STUB(20)
ISR_NO_ERR_STUB(21)
ISR_NO_ERR_STUB(22)
ISR_NO_ERR_STUB(23)
ISR_NO_ERR_STUB(24)
ISR_NO_ERR_STUB(25)
ISR_NO_ERR_STUB(26)
ISR_NO_ERR_STUB(27)
ISR_NO_ERR_STUB(28)
ISR_NO_ERR_STUB(29)
ISR_ERR_STUB(30)
ISR_NO_ERR_STUB(31)


extern void isr_stub_0(void);
extern void isr_stub_1(void);
extern void isr_stub_2(void);
extern void isr_stub_3(void);
extern void isr_stub_4(void);
extern void isr_stub_5(void);
extern void isr_stub_6(void);
extern void isr_stub_7(void);
extern void isr_stub_8(void);
extern void isr_stub_9(void);
extern void isr_stub_10(void);
extern void isr_stub_11(void);
extern void isr_stub_12(void);
extern void isr_stub_13(void);
extern void isr_stub_14(void);
extern void isr_stub_15(void);
extern void isr_stub_16(void);
extern void isr_stub_17(void);
extern void isr_stub_18(void);
extern void isr_stub_19(void);
extern void isr_stub_20(void);
extern void isr_stub_21(void);
extern void isr_stub_22(void);
extern void isr_stub_23(void);
extern void isr_stub_24(void);
extern void isr_stub_25(void);
extern void isr_stub_26(void);
extern void isr_stub_27(void);
extern void isr_stub_28(void);
extern void isr_stub_29(void);
extern void isr_stub_30(void);
extern void isr_stub_31(void);


/* External declarations for C handlers */
extern void isr_handler_c(uint32_t int_no, uint32_t err_code);
extern void irq0_c(void);
extern void irq1_c(void);

void (*isr_stubs[32])(void) = {
        isr_stub_0,  isr_stub_1,  isr_stub_2,  isr_stub_3,
        isr_stub_4,  isr_stub_5,  isr_stub_6,  isr_stub_7,
        isr_stub_8,  isr_stub_9,  isr_stub_10, isr_stub_11,
        isr_stub_12, isr_stub_13, isr_stub_14, isr_stub_15,
        isr_stub_16, isr_stub_17, isr_stub_18, isr_stub_19,
        isr_stub_20, isr_stub_21, isr_stub_22, isr_stub_23,
        isr_stub_24, isr_stub_25, isr_stub_26, isr_stub_27,
        isr_stub_28, isr_stub_29, isr_stub_30, isr_stub_31
};


/* IDT set descriptor function */
void idt_set_entry(uint8_t vector, void *isr, uint8_t flags)
{
        uintptr_t addr = (uintptr_t)isr;
        struct idt_entry* desc = &idt[vector];

        desc->isr_low = addr & 0xFFFF;
        desc->kernel_cs = 0x08;
        desc->reserved = 0;
        desc->attributes = flags;
        desc->isr_high = (addr >> 16) & 0xFFFF;
}


void idt_init()
{
        idtr.base = (uint32_t)&idt[0];
        idtr.limit = (sizeof(struct idt_entry) * IDT_SIZE) - 1;

        extern void irq0_handler();
        extern void irq1_handler();
        idt_set_entry(32, irq0_handler, 0x8E);
        idt_set_entry(33, irq1_handler, 0x8E);

        __asm__ __volatile__ ("lidt %0" :: "m"(idtr));

        kprintf("[  %%gOK%%w  ] Initialized IDT...\n");
}
