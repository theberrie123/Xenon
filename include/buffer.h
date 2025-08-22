#ifndef BUFFER_H
#define BUFFER_H


#include <xenon/type.h>

struct Buffer {
        char *data;
        size_t length;
        size_t allocated;
};

int resize_buffer(struct Buffer *buffer, size_t new_size);
void init_buffer(struct Buffer *buffer);
void add_char_buffer(struct Buffer *buffer, char c);
void clear_buffer(struct Buffer *buffer);
void free_buffer(struct Buffer *buffer);


#endif
