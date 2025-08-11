#include "../include/string.h"
#include "../include/type.h"


size_t strlen(const char *str)
{
        size_t len = 0;
        while (str[len] != '\0') {
                len++;
        }
        return len;
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
