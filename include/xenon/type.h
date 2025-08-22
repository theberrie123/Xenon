#ifndef CTYPE_H
#define CTYPE_H


typedef unsigned char uint8_t;
typedef unsigned short uint16_t;
typedef unsigned int uint32_t;
typedef unsigned long long uint64_t;

typedef unsigned long size_t;

typedef unsigned int uintptr_t;

#ifndef NULL
#define NULL ((void*)0)
#endif

typedef enum {
        BOOL_FALSE = 0,
        BOOL_TRUE  = 1
} bool_t;


#if defined(__SIZE_MAX__)
        #define SIZE_MAX __SIZE_MAX__
#elif defined(__x86_64__) || defined(_M_X64)
        #define SIZE_MAX 0xFFFFFFFFFFFFFFFFULL
#else
        #define SIZE_MAX 0xFFFFFFFFUL
#endif


#endif
