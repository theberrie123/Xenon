/*
* Chunk of memory (a "heap")
* Hand out parts of it when someone asks (malloc)
* Take parts back when someone frees (free)
*/


/*
* Heap memory comes from the reserved big chunk (4 MB at a known address)
* No freeing
*/


#include "xenon/memory.h"
#include "tty.h"

static struct heap_block *heap_head = NULL;

/* Merge adjacent free blocks */
void coalesce(struct heap_block *block)
{
    if (!block) return;

    /* merge with next if free and inside bounds */
    if (block->next && block->next->free &&
        (char *)block->next + sizeof(struct heap_block) + block->next->size <= heap_end)
    {
        block->size += sizeof(struct heap_block) + block->next->size;
        block->next = block->next->next;
        if (block->next) block->next->prev = block;
    }

    /* merge with prev if free and inside bounds */
    if (block->prev && block->prev->free &&
        (char *)block->prev >= heap_base)
    {
        block->prev->size += sizeof(struct heap_block) + block->size;
        block->prev->next = block->next;
        if (block->next) block->next->prev = block->prev;
        block = block->prev;
    }
}

/* Allocate memory from the kernel heap */
void *kmalloc(size_t size)
{
    if (size == 0) return NULL;

    size = (size + 7) & ~7; /* align to 8 bytes */

    /* search free blocks */
    struct heap_block *curr = heap_head;
    while (curr) {
        if (curr->free && curr->size >= size) {
            curr->free = 0;
            return (void *)(curr + 1);
        }
        curr = curr->next;
    }

    size_t required_space = size + sizeof(struct heap_block);

    /* check heap overflow */
    if (heap_ptr + required_space > heap_end) return NULL;

    struct heap_block *new_block = (struct heap_block *)heap_ptr;
    new_block->size = size;
    new_block->free = 0;
    new_block->next = NULL;
    new_block->prev = NULL;

    /* link into list */
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

/* Free memory */
void kfree(void *ptr)
{
    if (!ptr) return;

    struct heap_block *block = (struct heap_block *)ptr - 1;

    /* sanity check */
    if ((char *)block < heap_base || (char *)block >= heap_end) return;

    block->free = 1;

    coalesce(block);
}

/* Allocate zeroed memory */
void *kcalloc(size_t nmemb, size_t size)
{
    if (nmemb == 0 || size == 0) return NULL;

    size_t total = nmemb * size;
    if (total / nmemb != size) return NULL; /* overflow */

    void *ptr = kmalloc(total);
    if (!ptr) return NULL;

    char *p = ptr;
    for (size_t i = 0; i < total; i++) p[i] = 0;

    return ptr;
}

/* Reallocate memory */
void *krealloc(void *ptr, size_t new_size)
{
    if (!ptr) return kmalloc(new_size);
    if (new_size == 0) {
        kfree(ptr);
        return NULL;
    }

    struct heap_block *block = (struct heap_block *)ptr - 1;

    if (block->size >= new_size) return ptr;

    /* allocate new block */
    void *new_ptr = kmalloc(new_size);
    if (!new_ptr) return NULL;

    /* copy old data safely */
    char *src = ptr;
    char *dst = new_ptr;
    for (size_t i = 0; i < block->size; i++) dst[i] = src[i];

    kfree(ptr);
    return new_ptr;
}

/* Debug: print heap blocks */
void print_heap(void)
{
    struct heap_block *curr = heap_head;
    kprintf("Heap dump: base=%p, end=%p, ptr=%p\n", heap_base, heap_end, heap_ptr);
    if (!curr) {
        kprintf("Heap is empty.\n");
        return;
    }

    int index = 0;
    while (curr) {
        kprintf("[%d] Block %p: size=%zu, free=%s, prev=%p, next=%p\n",
               index,
               curr,
               curr->size,
               curr->free ? "yes" : "no",
               curr->prev,
               curr->next);
        curr = curr->next;
        index++;
    }
}
