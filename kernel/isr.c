#include "isr.h"
#include "paging.h"
#include "tty.h"
#include "idt.h"
#include "syscall.h"
#include "kernel.h"
#include "xenon/type.h"
#include "irq.h"


#define ISR_COUNT 32




struct registers {
        uint32_t eax;
        uint32_t ebx;
        uint32_t ecx;
        uint32_t edx;
};

struct registers regs;

#define SYS_WRITE 1


int isr80_handler_c()
{
        switch (regs.eax) {
                case SYS_WRITE:
                        int write_fd = regs.ebx;
                        const char *write_buf = (const char *)regs.ecx;
                        size_t write_count = (size_t)regs.edx;

                        /* stdout */
                        if (write_fd == 1) {
                                for (size_t i = 0; i < write_count; i++) {
                                        kputchar(write_buf[i]);
                                }
                        }

                        return write_count;
        }

        return -1;
}


void isr80_handler(void) __attribute__((naked));
void isr80_handler(void) {
        __asm__ __volatile__ (
                "cli\n\t"
                "pusha\n\t"
                "movl %%eax, %0\n\t"
                "movl %%ebx, %1\n\t"
                "movl %%ecx, %2\n\t"
                "movl %%edx, %3\n\t"
                "call isr80_handler_c\n\t"
                "popa\n\t"
                "sti\n\t"
                "iret\n\t"
                : "=m"(regs.eax), "=m"(regs.ebx), "=m"(regs.ecx), "=m"(regs.edx)
                :
                : "eax", "ebx", "ecx", "edx"
        );
}






static isr_handler_t interrupt_handlers[ISR_COUNT] = {0};

void isr_handler_c(struct regs *regs)
{
        if (regs->int_no < ISR_COUNT && interrupt_handlers[regs->int_no]) {
                interrupt_handlers[regs->int_no](regs);
                return;
        }

        panic("unhandled interrupt: %d, err: %x", regs->int_no, regs->err_code);
}




void isr_register_handler(uint8_t n, isr_handler_t handler)
{
    if (n < ISR_COUNT)
        interrupt_handlers[n] = handler;
}

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
}
