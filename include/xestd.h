#ifndef _XESTD_H
#define _XESTD_H


#include "xenon/type.h"


#define SYS_EXIT 0
#define SYS_WRITE 1


extern unsigned int syscall_handler(unsigned int num, unsigned int arg1, unsigned int arg2, unsigned int arg3);


ssize_t write(int fd, const void *buf, size_t count)
{
        int ret;
        __asm__ __volatile__ (
                "int $0x80"
                : "=a"(ret)
                : "a"(SYS_WRITE), "b"(fd), "c"(buf), "d"(count)
        );
        return ret;
}

void exit(int code)
{
        __asm__ __volatile__ (
                "int $0x80"
                :
                : "a"(SYS_EXIT), "b"(code)
        );
}


#endif /* _XESTD_H */
