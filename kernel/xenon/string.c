#include "xenon/string.h"
#include "xenon/type.h"
#include "xenon/arg.h"
#include "xenon/memory.h"


SIZE strlen(const char *s)
{
        SIZE len = 0;
        while (s[len] != '\0') {
                len++;
        }
        return len;
}

char *strdup(const char *s)
{
        SIZE len = strlen(s);

        char *copy = (char *)kmalloc(len + 1);
        for (SIZE i = 0; i <= len; i++) {
                copy[i] = s[i];
        }
        return copy;
}

int strncmp(const char *s1, const char *s2, SIZE n)
{
        for (SIZE i = 0; i < n; i++) {
                UINT8 c1 = (UINT8)s1[i];
                UINT8 c2 = (UINT8)s2[i];

                if (c1 != c2) {
                        return (int)c1 - (int)c2;
                }

                if (c1 == '\0') {
                        return 0;
                }
        }
        return 0;
}

char *strcpy(char *dest, const char *s)
{
        char *ret = dest;
        while (*s != '\0') {
                dest++;
                s++;
        }

        *dest = '\0';
        
        return ret;
}

int strcmp(const char *s1, const char *s2)
{
        while (*s1 && (*s1 == *s2)) {
                s1++;
                s2++;
        }
        return (UINT8)*s1 - (UINT8)*s2;
}

char *strcat(char *dest, const char *s)
{
        char *ret = dest;

        while (*dest != '\0') {
                dest++;
        }

        while (*s != '\0') {
                *dest = *s;
                dest++;
                s++;
        }

        *dest = '\0';

        return ret;
}

void *memset(void *dest, int value, SIZE count)
{
        UINT8 *ptr = (UINT8 *)dest;
        for (UINT32 i = 0; i < count; i++) {
                ptr[i] = (UINT8)value;
        }
        return dest;
}

void *memcpy(void *dest, const void *s, SIZE count)
{
        UINT8 *d = (UINT8 *)dest;
        const UINT8 *str = (const UINT8 *)s;
        for (UINT32 i = 0; i < count; i++) {
                d[i] = str[i];
        }
        return dest;
}

char *strncpy(char *dest, const char *s, SIZE n)
{
        for (unsigned i = 0; i < n && s[i] != '\0'; i++) {
                dest[i] = s[i];
        }
        for (SIZE i = 0; i < n; i++) {
                dest[i] = '\0';
        }
        return dest;
}

int memcmp(const void *s1, const void *s2, SIZE n)
{
        const UINT8 *p1 = (const UINT8 *)s1;
        const UINT8 *p2 = (const UINT8 *)s2;

        for (SIZE i = 0; i < n; i++) {
                if (p1[i] != p2[i]) {
                        return (p1[i] < p2[i]) ? -1 : 1;
                }
        }
        return 0;
}


void itoa(int value, char *s, int base)
{
        char buffer[32];
        int i = 0;
        int is_negative = 0;

        if (value == 0) {
                s[0] = '0';
                s[1] = '\0';
                return;
        }

        if (value < 0 && base == 10) {
                is_negative = 1;
                value = -value;
        }

        while (value != 0) {
                int rem = value % base;
                buffer[i++] = (rem > 9) ? (rem - 10 + 'a') : (rem + '0');
                value /= base;
        }

        if (is_negative) {
                buffer[i++] = '-';
        }

        for (int j = 0; j < i; j++) {
                s[j] = buffer[i - j - 1];
        }
        s[i] = '\0';
}

const char *strrchr(const char *s, char c)
{
        const char *last = NULL;
        while (*s) {
                if (*s == c) {
                        last = s;
                }
                s++;
        }
        return last;
}

unsigned long strtol(const char *s, char **endptr, int base)
{
        SIZE result = 0;
        char c;

        while ((c = *s++) != 0) {
                UINT32 val;
                if (c >= '0' && c <= '9') {
                        val = c - '0';
                } else if (c >= 'A' && c <= 'F') {
                        val = c - 'A' + 10;
                } else if (c >= 'a' && c <= 'f') {
                        val = c - 'a' + 10;
                } else {
                        break;
                }

                if (val >= (UINT32)base) {
                        break;
                }

                result = result * base + val;
        }
        if (endptr) {
                *endptr = (char *)s;
        }
        return result;
}


int snprintf(char *buf, SIZE size, 
                    const char *fmt, ...) {
    va_list args;
    va_start(args, fmt);

    SIZE pos = 0;
    for (SIZE i = 0; fmt[i] != '\0'; i++) {
        if (fmt[i] == '%' && fmt[i + 1] != '\0') {
            i++;
            char temp[64];

            switch (fmt[i]) {
                case 'c': {
                    char c = (char)va_arg(args, int);
                    if (pos + 1 < size) buf[pos] = c;
                    pos++;
                    break;
                }
                case 's': {
                    char *s = va_arg(args, char *);
                    for (SIZE j = 0; s[j] != '\0'; j++) {
                        if (pos + 1 < size) buf[pos] = s[j];
                        pos++;
                    }
                    break;
                }
                case 'd': {
                    int d = va_arg(args, int);
                    itoa(d, temp, 10);
                    for (SIZE j = 0; temp[j] != '\0'; j++) {
                        if (pos + 1 < size) buf[pos] = temp[j];
                        pos++;
                    }
                    break;
                }
                case 'x': {
                    int x = va_arg(args, int);
                    itoa(x, temp, 16);
                    for (SIZE j = 0; temp[j] != '\0'; j++) {
                        if (pos + 1 < size) buf[pos] = temp[j];
                        pos++;
                    }
                    break;
                }
                case '%': {
                    if (pos + 1 < size) buf[pos] = '%';
                    pos++;
                    break;
                }
                default:
                    if (pos + 1 < size) buf[pos] = fmt[i];
                    pos++;
            }
        } else {
            if (pos + 1 < size) buf[pos] = fmt[i];
            pos++;
        }
    }

    if (size > 0) {
        buf[(pos < size) ? pos : size - 1] = '\0';
    }

    va_end(args);
    return pos;
}

char *strtok(char *str, const char *delim)
{
    static char *next;
    if (str) next = str;

    if (!next) return NULL;

    while (*next) {
        int is_delim = 0;
        for (const char *d = delim; *d; d++) {
            if (*next == *d) {
                is_delim = 1;
                break;
            }
        }
        if (!is_delim) break;
        next++;
    }

    if (*next == '\0') return NULL;

    char *start = next;

    while (*next) {
        int is_delim = 0;
        for (const char *d = delim; *d; d++) {
            if (*next == *d) {
                is_delim = 1;
                break;
            }
        }
        if (is_delim) {
            *next = '\0';
            next++;
            return start;
        }
        next++;
    }

    next = NULL;
    return start;
}

char *strchr(const char *s, char c)
{
        while (*s) {
                if (*s == c) {
                        return (char *)s;
                }
                s++;
        }
        return 0;
}
