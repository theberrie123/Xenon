#include "../include/memory.h"
#include "../include/type.h"



mem_block_t *free_list = NULL;
void *heap_start = NULL;
size_t heap_size = 0;

void kheap_init()
{
        heap_start = (void *)&__heap_start;
        heap_size = (size_t)(&__heap_end - &__heap_start);

        free_list = (mem_block_t *)heap_start;
        free_list->size = heap_size - BLOCK_SIZE;
        free_list->next = NULL;
        free_list->free = 1;
}

void split_block(mem_block_t *block, size_t size)
{
        mem_block_t *new_block = (mem_block_t *)((char *)block + BLOCK_SIZE + size);
        new_block->size = block->size - size - BLOCK_SIZE;
        new_block->next = block->next;
        new_block->free = 1;

        block->size = size;
        block->next = new_block;
}

mem_block_t *find_free_block(size_t size)
{
        mem_block_t *curr = free_list;

        while (curr) {
                if (curr->free && curr->size >= size) {
                        return curr;
                }
                curr = curr->next;
        }
        return NULL;
}

void coalesce()
{
        mem_block_t *curr = free_list;

        while (curr && curr->next) {
                if (curr->free && curr->next->free) {
                        curr->size += BLOCK_SIZE + curr->next->size;
                        curr->next = curr->next->next;
                } else {
                        curr = curr->next;
                }
        }
}

void *kmalloc(size_t size)
{
        if (size == 0) {
                return NULL;
        }

        mem_block_t *block = find_free_block(size);
        if (!block) {
                return NULL;
        }

        if (block->size > size + BLOCK_SIZE) {
                split_block(block, size);
        }
        block->free = 0;

        return (void*)(block + 1);
}

void kfree(void *ptr)
{
        if (!ptr) {
                return;
        }

        mem_block_t *block = (mem_block_t *)ptr - 1;
        block->free = 1;

        coalesce();
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

        mem_block_t *block = (mem_block_t *)ptr - 1;

        if (block->size >= new_size) {
                return ptr;
        }

        void *new_ptr = kmalloc(new_size);
        if (!new_ptr) {
                return NULL;
        }

        size_t copy_size = block->size < new_size ? block->size : new_size;
        for (size_t i = 0; i < copy_size; i++) {
                ((char *)new_ptr)[i] = ((char *)ptr)[i];
        }

        kfree(ptr);

        return new_ptr;
}
