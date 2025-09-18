#include "xenon/memory.h"
#include "tty.h"

static struct heap_block *heap_head = NULL;


char *heap_base = &__heap_start;
char *heap_end = &__heap_end;
char *heap_ptr = &__heap_start;

void coalesce(struct heap_block *block)
{
        if (!block) {
                return;
        }

        if (block->next && block->next->free && (char *)block->next + sizeof(struct heap_block) + block->next->size <= heap_end) {
                block->size += sizeof(struct heap_block) + block->next->size;
                block->next = block->next->next;
                if (block->next) {
                        block->next->prev = block;
                }
        }

        if (block->prev && block->prev->free && (char *)block->prev >= heap_base) {
                block->prev->size += sizeof(struct heap_block) + block->size;
                block->prev->next = block->next;
                if (block->next) {
                        block->next->prev = block->prev;
                }
                block = block->prev;
        }
}

void *kmalloc(size_t size)
{
        if (size == 0) {
                return NULL;
        }

        size = (size + 7) & ~7;

        struct heap_block *curr = heap_head;
        while (curr) {
                if (curr->free && curr->size >= size) {
                        curr->free = 0;
                        return (void *)(curr + 1);
                }
                curr = curr->next;
        }

        size_t required_space = size + sizeof(struct heap_block);

        if (heap_ptr + required_space > heap_end) {
                return NULL;
        }

        struct heap_block *new_block = (struct heap_block *)heap_ptr;
        new_block->size = size;
        new_block->free = 0;
        new_block->next = NULL;
        new_block->prev = NULL;

        if (!heap_head) {
                heap_head = new_block;
        } else {
                curr = heap_head;
                while (curr->next) curr = curr->next;
                curr->next = new_block;
                new_block->prev = curr;
        }

        heap_ptr += required_space;

        return (void *)(new_block + 1);
}

void kfree(void *ptr)
{
        if (!ptr) {
                return;
        }
        struct heap_block *block = (struct heap_block *)ptr - 1;

        if ((char *)block < heap_base || (char *)block >= heap_end) {
                return;
        }

        block->free = 1;

        coalesce(block);
}

void *kcalloc(size_t nmemb, size_t size)
{
        if (nmemb == 0 || size == 0) {
                return NULL;
        }

        size_t total = nmemb * size;
        if (total / nmemb != size) {
                return NULL;
        }

        void *ptr = kmalloc(total);
        if (!ptr) {
                return NULL;
        }

        char *p = ptr;
        for (size_t i = 0; i < total; i++) {
                p[i] = 0;
        }
        return ptr;
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

        struct heap_block *block = (struct heap_block *)ptr - 1;

        if (block->size >= new_size) {
                return ptr;
        }

        void *new_ptr = kmalloc(new_size);
        if (!new_ptr) {
                return NULL;
        }

        char *src = ptr;
        char *dst = new_ptr;

        for (size_t i = 0; i < block->size; i++) {
                dst[i] = src[i];
        }

        kfree(ptr);
        return new_ptr;
}
