#ifndef STRING_H
#define STRING_H


#include "type.h"


size_t strlen(const char *str);
char *strcpy(char *dest, const char *src);
int strcmp(const char *s1, const char *s2);
char *strcat(char *dest, const char *src);

void *memset(void *dest, int value, size_t count);
void *memcpy(void *dest, const void *src, size_t count);
char *strncpy(char *dest, const char *src, size_t n);

#endif
