#include "../include/isr.h"
#include "../include/paging.h"
#include "../include/tty.h"
#include "../include/idt.h"


#define ISR_COUNT 32


static isr_handler_t interrupt_handlers[ISR_COUNT] = {0};

void isr_handler_c(registers_t *regs)
{
    if (regs->int_no < ISR_COUNT && interrupt_handlers[regs->int_no]) {
        interrupt_handlers[regs->int_no](regs);
        return;
    }

        kprintf("Unhandled interrupt: %d, err: %x\n", regs->int_no, regs->err_code);
        kprintf("EIP=%x CS=%x EFLAGS=%x\n", regs->eip, regs->cs, regs->eflags);
        kprintf("EAX=%x EBX=%x ECX=%x EDX=%x\n", regs->eax, regs->ebx, regs->ecx, regs->edx);
}


void isr_register_handler(uint8_t n, isr_handler_t handler)
{
    if (n < ISR_COUNT)
        interrupt_handlers[n] = handler;
}

// Install CPU exceptions into the IDT
extern void isr_stub_0(void);  extern void isr_stub_1(void);
extern void isr_stub_2(void);  extern void isr_stub_3(void);
extern void isr_stub_4(void);  extern void isr_stub_5(void);
extern void isr_stub_6(void);  extern void isr_stub_7(void);
extern void isr_stub_8(void);  extern void isr_stub_9(void);
extern void isr_stub_10(void); extern void isr_stub_11(void);
extern void isr_stub_12(void); extern void isr_stub_13(void);
extern void isr_stub_14(void); extern void isr_stub_15(void);
extern void isr_stub_16(void); extern void isr_stub_17(void);
extern void isr_stub_18(void); extern void isr_stub_19(void);
extern void isr_stub_20(void); extern void isr_stub_21(void);
extern void isr_stub_22(void); extern void isr_stub_23(void);
extern void isr_stub_24(void); extern void isr_stub_25(void);
extern void isr_stub_26(void); extern void isr_stub_27(void);
extern void isr_stub_28(void); extern void isr_stub_29(void);
extern void isr_stub_30(void); extern void isr_stub_31(void);

static void* isr_stubs[ISR_COUNT] = {
    isr_stub_0, isr_stub_1, isr_stub_2, isr_stub_3,
    isr_stub_4, isr_stub_5, isr_stub_6, isr_stub_7,
    isr_stub_8, isr_stub_9, isr_stub_10, isr_stub_11,
    isr_stub_12, isr_stub_13, isr_stub_14, isr_stub_15,
    isr_stub_16, isr_stub_17, isr_stub_18, isr_stub_19,
    isr_stub_20, isr_stub_21, isr_stub_22, isr_stub_23,
    isr_stub_24, isr_stub_25, isr_stub_26, isr_stub_27,
    isr_stub_28, isr_stub_29, isr_stub_30, isr_stub_31
};

void isr_init()
{
        for (uint8_t i = 0; i < ISR_COUNT; i++) {
                idt_set_entry(i, isr_stubs[i], 0x8E);
        }
        kprintf("[  %%gOK%%w  ] Initialized ISR...\n");
}
