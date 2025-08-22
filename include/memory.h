#ifndef MEMORY_H
#define MEMORY_H


#include <xenon/type.h>


#define ALIGNMENT       16UL
#define MAGIC           0xC0FFEE00u


typedef struct mem_block {
        /*
         * payload size in bytes (not including header)
         * guard word
         * 1 = free, 0 = used
         * pad to keep header 16-byte aligned
         * next block in address order
         */
        size_t size;
        uint32_t magic;
        uint8_t free;
        uint8_t _pad[3];
        struct mem_block *next;
} mem_block_t;


#define BLOCK_SIZE   (sizeof(mem_block_t))

/* Alignment helper */
#define ALIGN_UP(x,a)   (((x) + ((a)-1)) & ~((a)-1))


extern uint8_t __heap_start;
extern uint8_t __heap_end;


static inline void kheap_lock()
{
        __asm__ __volatile__ ("cli");
}

static inline void kheap_unlock()
{
        __asm__ __volatile__ ("sti");
}


void kheap_init();
void *kmalloc(size_t size);
void kfree(void *ptr);
void *krealloc(void *ptr, size_t new_size);
void *kcalloc(size_t n, size_t size);


#endif /* MEMORY_H */
