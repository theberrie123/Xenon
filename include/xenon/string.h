#ifndef STRING_H
#define STRING_H


#include "xenon/type.h"
#include "xenon/arg.h"


size_t strlen(const char *s);
char *strdup(const char *s);
int strncmp(const char *s1, const char *s2, size_t n);
char *strcpy(char *dest, const char *s);
int strcmp(const char *s1, const char *s2);
char *strdup(const char *s);
char *strcat(char *dest, const char *s);
char *strncpy(char *dest, const char *s, size_t n);


void *memset(void *dest, int value, size_t count);
void *memcpy(void *dest, const void *s, size_t count);
int memcmp(const void *s1, const void *s2, size_t n);

void itoa(int value, char *s, int base);

const char *strrchr(const char *s, char c);

size_t strtol(const char *s, char **endptr, int base);

int snprintf(char *buf, size_t size, const char *fmt, ...);

char *strtok(char *str, const char *delim);
char *strchr(const char *s, char c);

#endif
