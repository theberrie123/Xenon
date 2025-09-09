#ifndef TYPES_H
#define TYPES_H


typedef unsigned char           uint8_t;
typedef unsigned short          uint16_t;
typedef unsigned int            uint32_t;
typedef unsigned long long      uint64_t;

typedef signed char             int8_t;
typedef signed short            int16_t;
typedef signed int              int32_t;
typedef signed long long        int64_t;

typedef unsigned int            uintptr_t;
typedef signed int              intptr_t;



typedef enum {
    BOOL_FALSE = 0,
    BOOL_TRUE  = 1
} bool_t;

#ifndef NULL
#define NULL ((void*)0)
#endif

typedef unsigned long           size_t;
typedef long                    ssize_t;

#if defined(__SIZE_MAX__)
    #define SIZE_MAX __SIZE_MAX__
#elif defined(__x86_64__) || defined(_M_X64)
    #define SIZE_MAX 0xFFFFFFFFFFFFFFFFULL
#else
    #define SIZE_MAX 0xFFFFFFFFUL
#endif


#endif /* TYPES_H */
