#ifndef MEMORY_H
#define MEMORY_H


#include "xenon/type.h"


extern char __heap_start, __heap_end;

extern char *heap_base;
extern char *heap_end;
extern char *heap_ptr;


struct heap_block {
        size_t size;
        int free;
        struct heap_block *next;
        struct heap_block *prev;
};


void *kmalloc(size_t size);
void kfree(void *ptr);
void *kcalloc(size_t nmemb, size_t size);
void *krealloc(void *ptr, size_t new_size);
void print_heap();


#endif
