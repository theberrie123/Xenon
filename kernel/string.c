#include "../include/string.h"


size_t strlen(const char *str)
{
        size_t len = 0;
        while (str[len] != '\0') {
                len++;
        }
        return len;
}

int strncmp(const char *s1, const char *s2, uint32_t n) {
    for (uint32_t i = 0; i < n; i++) {
        unsigned char c1 = (unsigned char)s1[i];
        unsigned char c2 = (unsigned char)s2[i];

        if (c1 != c2) return c1 - c2;
        if (c1 == '\0') return 0;
    }
    return 0;
}


char *strcpy(char *dest, const char *src) 
{
        char *ret = dest;
        while (*src != '\0') {
                dest++;
                src++;
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
        return (unsigned char)*s1 - (unsigned char)*s2;
}

char *strcat(char *dest, const char *src)
{
        char *ret = dest;

        while (*dest != '\0') {
                dest++;
        }

        while (*src != '\0') {
                *dest = *src;
                dest++;
                src++;
        }

        *dest = '\0';

        return ret;
}



void *memset(void *dest, int value, size_t count)
{
        unsigned char *ptr = (unsigned char *)dest;
        for (unsigned int i = 0; i < count; i++) {
                ptr[i] = (unsigned char)value;
        }
        return dest;
}

void *memcpy(void *dest, const void *src, size_t count)
{
        unsigned char *d = (unsigned char *)dest;
        const unsigned char *s = (const unsigned char *)src;
        for (unsigned int i = 0; i < count; i++) {
                d[i] = s[i];
        }
        return dest;
}

char *strncpy(char *dest, const char *src, size_t n)
{
        unsigned int i;
        for (i = 0; i < n && src[i] != '\0'; i++) {
                dest[i] = src[i];
        }
        for (; i < n; i++) {
                dest[i] = '\0';
        }
        return dest;
}

int memcmp(const void *s1, const void *s2, size_t n)
{
        const unsigned char *p1 = (const unsigned char *)s1;
        const unsigned char *p2 = (const unsigned char *)s2;

        for (size_t i = 0; i < n; i++) {
                if (p1[i] != p2[i]) {
                        return (p1[i] < p2[i]) ? -1 : 1;
                }
        }
        return 0;
}
