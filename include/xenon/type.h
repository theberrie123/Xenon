#ifndef TYPES_H
#define TYPES_H


typedef unsigned char UINT8;
typedef unsigned short UINT16;
typedef unsigned int UINT32;
typedef unsigned long long UINT64;

typedef signed char INT8;
typedef signed short INT16;
typedef signed int INT32;
typedef signed long long INT64;

typedef unsigned long SIZE;
typedef long SSIZE;


typedef enum {
    BOOL_FALSE = 0,
    BOOL_TRUE  = 1
} BOOL;

#ifndef NULL
#define NULL ((void*)0)
#endif


#if defined(__SIZE_MAX__)
    #define SIZE_MAX __SIZE_MAX__
#elif defined(__x86_64__) || defined(_M_X64)
    #define SIZE_MAX 0xFFFFFFFFFFFFFFFFULL
#else
    #define SIZE_MAX 0xFFFFFFFFUL
#endif


#endif /* TYPES_H */
