#ifndef MEMORY_H
#define MEMORY_H


#include "xenon/type.h"


#define ALIGNMENT       16UL
#define MAGIC           0xC0FFEE00u


typedef struct mem_block {
        SIZE size;
        UINT32 magic;
        UINT8 free;
        UINT8 _pad[3];
        struct mem_block *next;
} mem_block_t;


#define BLOCK_SIZE   (sizeof(mem_block_t))

#define ALIGN_UP(x,a)   (((x) + ((a)-1)) & ~((a)-1))


extern UINT8 __heap_start;
extern UINT8 __heap_end;


static inline void kheap_lock()
{
        __asm__ __volatile__ ("cli");
}

static inline void kheap_unlock()
{
        __asm__ __volatile__ ("sti");
}


void kheap_init();
void *kmalloc(SIZE size);
void kfree(void *ptr);
void *krealloc(void *ptr, SIZE new_size);
void *kcalloc(SIZE n, SIZE size);


#endif /* MEMORY_H */
