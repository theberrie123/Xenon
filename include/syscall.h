#ifndef SYSCALL_H
#define SYSCALL_H


#include "tty.h"


#define SYS_WRITE 1


typedef unsigned int (*syscall_fn_t)(unsigned int, unsigned int, unsigned int);


unsigned int sys_write(unsigned int fd, unsigned int buf_ptr, unsigned int count)
{
        (void)fd;
        const char *buf = (const char *)buf_ptr;
        for (unsigned int i = 0; i < count; i++) {
                kputchar(buf[i]);
        }
        return count;
}

syscall_fn_t syscall_table[1] = {
        sys_write
};


unsigned int syscall_dispatch(unsigned int num,
                                unsigned int arg1,
                                unsigned int arg2,
                                unsigned int arg3) {
        if (num < 1) {
                return syscall_table[num](arg1, arg2, arg3);
        }
        return -1;
}

#endif /* SYSCALL_H */
