#ifndef MEMORY_H
#define MEMORY_H


#include "xenon/type.h"


extern char __heap_start, __heap_end;

extern char *heap_base;
extern char *heap_end;
extern char *heap_ptr;


struct heap_block {
        SIZE size;
        int free;
        struct heap_block *next;
        struct heap_block *prev;
};


void *kmalloc(SIZE size);
void kfree(void *ptr);
void *kcalloc(SIZE nmemb, SIZE size);
void *krealloc(void *ptr, SIZE new_size);
void print_heap();


#endif /* MEMORY_H */
