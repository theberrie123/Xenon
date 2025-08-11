#ifndef MEMORY_H
#define MEMORY_H

#include "type.h"


typedef struct mem_block {
        size_t size;
        struct mem_block *next;
        int free;
} mem_block_t;

extern mem_block_t *free_list;
extern void *heap_start;
extern size_t heap_size;


#define BLOCK_SIZE sizeof(mem_block_t)

extern mem_block_t *free_list;

extern char __heap_start;
extern char __heap_end;


void *kmalloc(size_t size);
void kfree(void *ptr);
void *krealloc(void *ptr, size_t new_size);

#endif
