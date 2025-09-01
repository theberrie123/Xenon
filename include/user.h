#ifndef USER_H
#define USER_H


#include "xenon/type.h"


/* Prototype: entry = virtual entry point for user, user_stack = virtual user stack top */
__attribute__((naked)) void enter_userspace(uint32_t entry, uint32_t user_stack)
{
    asm volatile(
        /* arg layout (naked fn): [esp] = return addr, [esp+4] = entry, [esp+8] = user_stack */
        "mov  4(%esp), %eax       \n" /* eax = entry */
        "mov  8(%esp), %ebx       \n" /* ebx = user_stack */

        /* load user data selectors into segment regs (RPL=3 selectors) */
        "movw $0x23, %ax          \n"
        "movw %ax, %ds            \n"
        "movw %ax, %es            \n"
        "movw %ax, %fs            \n"
        "movw %ax, %gs            \n"

        /* prepare stack frame for iret */
        "pushl $0x23              \n" /* SS (user data selector, RPL=3) */
        "pushl %ebx               \n" /* ESP (user stack) */
        "pushf                    \n" /* push EFLAGS */
        "popl %ecx                \n"
        "orl  $0x200, %ecx        \n" /* set IF = 1 so interrupts enabled in user */
        "pushl %ecx               \n"
        "pushl $0x1B              \n" /* CS (user code selector, RPL=3) */
        "pushl %eax               \n" /* EIP = entry */
        "iret                     \n"
    );
}



#endif
