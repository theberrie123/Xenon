#ifndef TTY_H
#define TTY_H


#include "xenon/type.h"


#define VGA_WIDTH 80
#define VGA_HEIGHT 25


#define WHITE_ON_BLACK  0x0F
#define GREEN_ON_BLACK  0x0A
#define YELLOW_ON_BLACK 0x0E
#define RED_ON_BLACK    0x0C
#define BLUE_ON_BLACK   0x01
#define CYAN_ON_BLACK   0x03

#define LIGHT_GRAY_ON_BLACK 0x07


extern UINT32 *framebuffer;

void kinit();
void kputchar(char c);
void kclear();
void tty_set_color(UINT8 color);
void tty_move_cursor(SIZE row, SIZE col);

void kprintf(const char *format, ...);


void kexit(int code);

void update_cursor();
void putchar_update_cursor(char c);

void move_cursor(int dx, int dy);
void draw_cursor(int x, int y);


void cursor_backward();

__attribute__ ((noreturn))
void panic(const char *format, ...);


#endif /* TTY_H */
