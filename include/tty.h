#ifndef TTY_H
#define TTY_H


#include "type.h"


#define VGA_WIDTH 80
#define VGA_HEIGHT 25


extern unsigned int *framebuffer;

void kinit();
void kputchar(char c);
void kclear();
void tty_set_color(uint8_t fg, uint8_t bg);
void tty_move_cursor(size_t row, size_t col);

void kprintf(const char *format, ...);

void kexit(int code);

void update_cursor();
void putchar_update_cursor(char c);

void move_cursor(int dx, int dy);
void draw_cursor(int x, int y);


#endif
