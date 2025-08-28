#ifndef SYSCALL_H
#define SYSCALL_H


#define SYS_EXIT 0
#define SYS_WRITE 1
#define SYS_READ 2



unsigned int syscall_handler(unsigned int num, unsigned int arg1, unsigned int arg2, unsigned int arg3, unsigned int arg4, unsigned int arg5);


#endif /* SYSCALL_H */
