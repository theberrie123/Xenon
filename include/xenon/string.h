#ifndef STRING_H
#define STRING_H


#include "xenon/type.h"
#include "xenon/arg.h"
#include "xenon/memory.h"

size_t strlen(const char *str);
char *strcpy(char *dest, const char *src);
int strcmp(const char *s1, const char *s2);
char *strcat(char *dest, const char *src);

static char* strdup(const char* s) {
    size_t len = 0;
    while (s[len]) len++;
    char* copy = (char*)kmalloc(len + 1);
    for (size_t i = 0; i <= len; i++) copy[i] = s[i];
    return copy;
}

int strncmp(const char *s1, const char *s2, uint32_t n);

void *memset(void *dest, int value, size_t count);
void *memcpy(void *dest, const void *src, size_t count);
char *strncpy(char *dest, const char *src, size_t n);
int memcmp(const void *s1, const void *s2, size_t n);

static void itoa(int value, char *str, int base);


// Helper function to convert integer to string
static void itoa(int value, char *str, int base) {
    char buffer[32];
    int i = 0;
    int is_negative = 0;

    if (value == 0) {
        str[0] = '0';
        str[1] = '\0';
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

    // Reverse the string
    int j;
    for (j = 0; j < i; j++) {
        str[j] = buffer[i - j - 1];
    }
    str[i] = '\0';
}

// Custom snprintf
static int snprintf(char *buf, size_t size, const char *fmt, ...) {
    va_list args;
    va_start(args, fmt);

    size_t pos = 0;
    for (size_t i = 0; fmt[i] != '\0'; i++) {
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
                    for (size_t j = 0; s[j] != '\0'; j++) {
                        if (pos + 1 < size) buf[pos] = s[j];
                        pos++;
                    }
                    break;
                }
                case 'd': {
                    int d = va_arg(args, int);
                    itoa(d, temp, 10);
                    for (size_t j = 0; temp[j] != '\0'; j++) {
                        if (pos + 1 < size) buf[pos] = temp[j];
                        pos++;
                    }
                    break;
                }
                case 'x': {
                    int x = va_arg(args, int);
                    itoa(x, temp, 16);
                    for (size_t j = 0; temp[j] != '\0'; j++) {
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
                    // Unsupported specifier, just print it
                    if (pos + 1 < size) buf[pos] = fmt[i];
                    pos++;
            }
        } else {
            if (pos + 1 < size) buf[pos] = fmt[i];
            pos++;
        }
    }

    // Null terminate
    if (size > 0) {
        buf[(pos < size) ? pos : size - 1] = '\0';
    }

    va_end(args);
    return pos;
}

static const char* strchr(const char* s, char c) {
    while (*s) {
        if (*s == c) return s;
        s++;
    }
    return NULL;
}

static const char* strrchr(const char* s, char c) {
    const char* last = NULL;
    while (*s) {
        if (*s == c) last = s;
        s++;
    }
    return last;
}

static unsigned long strtol(const char *str, char **endptr, int base) {
    unsigned long result = 0;
    char c;
    while ((c = *str++) != 0) {
        unsigned int val;
        if (c >= '0' && c <= '9') val = c - '0';
        else if (c >= 'A' && c <= 'F') val = c - 'A' + 10;
        else if (c >= 'a' && c <= 'f') val = c - 'a' + 10;
        else break;

        if (val >= (unsigned int)base) break;

        result = result * base + val;
    }
    if (endptr) *endptr = (char*)str;
    return result;
}

#endif /* STRING_H */
