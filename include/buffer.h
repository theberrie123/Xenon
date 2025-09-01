#ifndef BUFFER_H
#define BUFFER_H


#include "xenon/type.h"

struct buffer {
        char *data;
        size_t length;
        size_t allocated;
};

int resize_buffer(struct buffer *buffer, size_t new_size);
void init_buffer(struct buffer *buffer);
void add_char_buffer(struct buffer *buffer, char c);
void clear_buffer(struct buffer *buffer);
void free_buffer(struct buffer *buffer);


#endif
