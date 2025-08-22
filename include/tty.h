#ifndef TTY_H
#define TTY_H


#include <xenon/type.h>

#define VGA_WIDTH 80
#define VGA_HEIGHT 25


#define WHITE_ON_BLACK  0x0F
#define GREEN_ON_BLACK  0x0A
#define YELLOW_ON_BLACK 0x0E
#define RED_ON_BLACK    0x0C


extern unsigned int *framebuffer;

void kinit();
void kputchar(char c);
void kclear();
void tty_set_color(uint8_t color);
void tty_move_cursor(size_t row, size_t col);

void kprintf(const char *format, ...);

void kexit(int code);

void update_cursor();
void putchar_update_cursor(char c);

void move_cursor(int dx, int dy);
void draw_cursor(int x, int y);


#endif
