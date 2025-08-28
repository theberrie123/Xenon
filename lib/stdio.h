#ifndef _STDIO_H
#define _STDIO_H


#include "xenon/string.h"

#include <xestd.h>


typedef __builtin_va_list va_list;
#define va_start(ap, last) __builtin_va_start(ap, last)
#define va_arg(ap, type) __builtin_va_arg(ap, type)
#define va_end(ap) __builtin_va_end(ap)



int printf(const char *format, ...)
{
        va_list args;
        va_start(args, format);

        for (const char *p = format; *p != '\0'; p++) {
                if (*p == '%') {
                        p++;
                        switch (*p) {
                                case 'c':
                                        char c = (char)va_arg(args, int);
                                        write(1, &c, 1);
                                        break;
                                case 's':
                                        const char *s = va_arg(args, const char *);
                                        int count = strlen(s);
                                        write(1, s, count);
                                        break;
                        }
                } else {
                        int count = strlen(p);
                        write(1, &*p, count);
                }
        }

        va_end(args);
}


#endif /* _STDIO_H */
