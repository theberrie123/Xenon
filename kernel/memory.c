#include "xenon/memory.h"
#include "xenon/type.h"
#include "xenon/string.h"


static mem_block_t *free_list = NULL;
static uint8_t *heap_begin = NULL;
static uint8_t *heap_end = NULL;



static inline int blocks_are_adjacent(mem_block_t *a,
                                      mem_block_t *b)
{
        uint8_t *a_end = (uint8_t*)a + BLOCK_SIZE + a->size;
        return a_end == (uint8_t*)b;
}

static void coalesce_list()
{
        mem_block_t* cur = free_list;
        while (cur && cur->next) {
                if (cur->free && cur->next->free && blocks_are_adjacent(cur, cur->next)) {
                        cur->size += BLOCK_SIZE + cur->next->size;
                        cur->next = cur->next->next;
                } else {
                        cur = cur->next;
                }
        }
}

static mem_block_t *find_suitable(size_t size)
{
        mem_block_t *cur = free_list;
        while (cur) {
                if (cur->free && cur->size >= size) {
                        return cur;
                }
                cur = cur->next;
        }
        return NULL;
}

static void split_block(mem_block_t *block, size_t size)
{
        if (block->size < size + BLOCK_SIZE + ALIGNMENT) {
                return;
        }

        uint8_t *new_addr = (uint8_t*)block + BLOCK_SIZE + size;
        mem_block_t *new_block = (mem_block_t*)new_addr;

        new_block->size = block->size - size - BLOCK_SIZE;
        new_block->magic = MAGIC;
        new_block->free = 1;
        new_block->next = block->next;

        block->size = size;
        block->next = new_block;
}

void kheap_init()
{
        heap_begin = &__heap_start;
        heap_end = &__heap_end;

        uintptr_t aligned_start = ALIGN_UP((uintptr_t)heap_begin, ALIGNMENT);
        size_t total = (size_t)((uintptr_t)heap_end - aligned_start);
        if (total < BLOCK_SIZE + ALIGNMENT) {
                free_list = NULL; /* too small; prevents allocation */
                return;
        }

        free_list = (mem_block_t*)aligned_start;
        free_list->size = total - BLOCK_SIZE;
        free_list->magic = MAGIC;
        free_list->free = 1;
        free_list->next = NULL;
}

void *kmalloc(size_t size)
{
        if (size == 0) {
                return NULL;
        }

        kheap_lock();

        size = ALIGN_UP(size, ALIGNMENT);

        mem_block_t *blk = find_suitable(size);
        if (!blk) {
                kheap_unlock();
                return NULL; /* out of memory (no grow yet) */
        }

        if (blk->magic != MAGIC) {
                kheap_unlock();
                return NULL; /* corruption detected */
        }

        split_block(blk, size);
        blk->free = 0;

        void *payload = (void*)((uint8_t*)blk + BLOCK_SIZE);
        kheap_unlock();
        return payload;
}

void kfree(void *ptr)
{
        if (!ptr) {
                return;
        }

        kheap_lock();

        mem_block_t *blk = (mem_block_t*)((uint8_t*)ptr - BLOCK_SIZE);

        if (blk->magic != MAGIC) {
                kheap_unlock();
                return;
        }
        if ((uint8_t*)blk < heap_begin || (uint8_t*)blk >= heap_end) {
                kheap_unlock();
                return;
        }

        blk->free = 1;
        coalesce_list();

        kheap_unlock();
}

void *krealloc(void *ptr, size_t new_size)
{
        if (!ptr) {
                return kmalloc(new_size);
        }
        if (new_size == 0) {
                kfree(ptr);
                return NULL;
        }

        new_size = ALIGN_UP(new_size, ALIGNMENT);

        kheap_unlock();

        mem_block_t *blk = (mem_block_t*)((uint8_t*)ptr - BLOCK_SIZE);
        if (blk->magic != MAGIC) {
                kheap_unlock();
                return NULL;
        }

        if (blk->size >= new_size) {
                split_block(blk, new_size);
                kheap_unlock();
                return ptr;
        }

        if (blk->next && blk->next->free && blocks_are_adjacent(blk, blk->next)) {
                blk->size += BLOCK_SIZE + blk->next->size;
                blk->next = blk->next->next;

                if (blk->size >= new_size) {
                        split_block(blk, new_size);
                        kheap_unlock();
                        return ptr;
                }
        }

        size_t old_size = blk->size;
        kheap_unlock();

        void *new_ptr = kmalloc(new_size);
        if (!new_ptr) {
                return NULL;
        }

        memcpy(new_ptr, ptr, old_size);
        kfree(ptr);
        return new_ptr;
}

void *kcalloc(size_t n, size_t size)
{
        if (n && size > (SIZE_MAX / n)) {
                return NULL;
        }

        size_t total = n * size;
        void *p = kmalloc(total);
        if (!p) {
                return NULL;
        }
        memset(p, 0, total);
        return p;
}
