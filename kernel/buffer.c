#include "../include/buffer.h"
#include "../include/tty.h"

#include "xenon/memory.h"


int resize_buffer(struct Buffer *buffer, size_t new_size)
{
        size_t new_allocated = (size_t)(new_size * 1.125) + 6;
        if (new_allocated < new_size + 1) {
                new_allocated = new_size + 1;
        }
        char *new_buffer = krealloc(buffer->data, new_allocated);
        if (!new_buffer) {
                return 1;
        }

        buffer->data = new_buffer;
        buffer->allocated = new_allocated;
        return 0;
}

void init_buffer(struct Buffer *buffer)
{
        buffer->data = kmalloc(32);
        if (!buffer->data) {
                kprintf("init_buffer ERROR: Out of memory\n");
        }


        buffer->length = 0;
        buffer->allocated = 32;
        buffer->data[0] = '\0';
}

void add_char_buffer(struct Buffer *buffer, char c)
{
        if (buffer->length + 1 >= buffer->allocated) {
                if (resize_buffer(buffer, buffer->length + 1) != 0) {
                        kprintf("add_char_buffer ERROR: Out of memory\n");
                }
        }
        buffer->data[buffer->length++] = c;
        buffer->data[buffer->length] = '\0';
}

void clear_buffer(struct Buffer *buffer)
{
        buffer->length = 0;
        if (buffer->data) {
                buffer->data[0] = '\0';
        }
}

void free_buffer(struct Buffer *buffer)
{
        kfree(buffer->data);
        buffer->data = NULL;
        buffer->length = 0;
        buffer->allocated = 0;
}
