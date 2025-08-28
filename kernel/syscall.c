#include "syscall.h"
#include "tty.h"


typedef unsigned int (*syscall_fn_t)(unsigned int, unsigned int, unsigned int, unsigned int, unsigned int);


unsigned int sys_write(int fd, const void *buf, size_t count)
{
        int ret;
        __asm__ __volatile__ (
                "int $0x80"
                : "=a"(ret)
                : "a"(SYS_WRITE), "b"(buf), "c"(count)
        );
        return ret;
}


static unsigned int write_wrapper(unsigned int arg1, unsigned int arg2, unsigned int arg3, unsigned int arg4, unsigned int arg5)
{
        return sys_write(arg1, (const char *)arg2, arg3);
}

static unsigned int exit_wrapper(unsigned int arg1, unsigned int arg2, unsigned int arg3, unsigned int arg4, unsigned int arg5)
{
        (void)arg1;
        (void)arg2;
        (void)arg3;

        return -1; /* unknown syscall */
}



#define NUM_SYSCALLS 5
static syscall_fn_t syscall_table[NUM_SYSCALLS] = {
        [SYS_EXIT] = exit_wrapper,
        [SYS_WRITE] = write_wrapper,
};


unsigned int syscall_handler(unsigned int num, unsigned int arg1, unsigned int arg2, unsigned int arg3, unsigned int arg4, unsigned int arg5)
{
        if (num >= NUM_SYSCALLS || syscall_table[num] == NULL) {
                return -1; /* unknown syscall */
        }
        return syscall_table[num](arg1, arg2, arg3, arg4, arg5);
}
