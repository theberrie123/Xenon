#ifndef MEMORY_H
#define MEMORY_H


#include "xenon/type.h"


extern char __heap_start, __heap_end;

static char *heap_base = &__heap_start;
static char *heap_end  = &__heap_end;
static char *heap_ptr  = &__heap_start;


struct heap_block {
        size_t size;
        int free; /* 1 = free, 0 = used */
        struct heap_block *next;
        struct heap_block *prev;
};


void *kmalloc(size_t size);
void kfree(void *ptr);
void *kcalloc(size_t nmemb, size_t size);
void *krealloc(void *ptr, size_t new_size);

void print_heap(void);

#endif /* MEMORY_H */
