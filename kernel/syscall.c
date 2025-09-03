#include "syscall.h"
#include "xenon/type.h"
#include "tty.h"


void syscall_handler(struct registers *regs)
{
        switch (regs->eax) {
                /*
                * ssize_t write(int fd, const void *buf, 
                *               size_t count)
                */
                case SYS_WRITE:
                        for (int i = 0; i < regs->ecx; i++) {
                                kprintf("%s", (char *)regs->ebx);
                        }
                        regs->eax = 0;
                        break;
                default:
                        kprintf("[  %%rERR%%w ]  unknown syscall %d\n", regs->eax);

        }
}
