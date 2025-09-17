#ifndef STRING_H
#define STRING_H


#include "xenon/type.h"
#include "xenon/arg.h"


SIZE strlen(const char *s);
char *strdup(const char *s);
int strncmp(const char *s1, const char *s2, SIZE n);
char *strcpy(char *dest, const char *s);
int strcmp(const char *s1, const char *s2);
char *strdup(const char *s);
char *strcat(char *dest, const char *s);
char *strncpy(char *dest, const char *s, SIZE n);


void *memset(void *dest, int value, SIZE count);
void *memcpy(void *dest, const void *s, SIZE count);
int memcmp(const void *s1, const void *s2, SIZE n);

void itoa(int value, char *s, int base);

const char *strrchr(const char *s, char c);

SIZE strtol(const char *s, char **endptr, int base);

int snprintf(char *buf, SIZE size, const char *fmt, ...);

char *strtok(char *str, const char *delim);
char *strchr(const char *s, char c);

#endif /* STRING_H */
