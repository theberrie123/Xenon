#ifndef XESTD_H
#define XESTD_H



static inline void sys_write(const char *buf, unsigned int count) {
    __asm__ __volatile__ (
        "int $0x90"
        :
        : "a"(1),      // syscall number = SYS_WRITE
          "b"(1),      // fd = stdout
          "c"(buf),    // buffer
          "d"(count)   // length
    );
}


#endif /* XESTD_H */
