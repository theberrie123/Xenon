#ifndef SYSCALL_H
#define SYSCALL_H


#include "xenon/type.h"


#define SYS_EXIT 0
#define SYS_WRITE 1


struct registers {
        uint32_t ds;
        uint32_t edi, esi, ebp, esp, ebx, edx, ecx, eax;
        uint32_t int_no, err_code;
        uint32_t eip, cs, eflags, useresp, ss;
};


#endif
