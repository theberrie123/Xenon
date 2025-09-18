#include "tty.h"
#include "string.h"
#include "io.h"
#include "xenon/arg.h"
#include "xenon/type.h"


#define CURSOR_SIZE 10

static volatile uint16_t *VGA_MEMORY = (volatile uint16_t *)0xB8000;

static size_t cursor_row = 0;
static size_t cursor_col = 0;

static uint8_t KCOLOR = 0x0F;

#define VGA_WIDTH 80
#define VGA_HEIGHT 25


static inline uint16_t vga_entry(uint8_t c, uint8_t color)
{
        return (uint16_t)c | ((uint16_t)color << 8);
}



static void kscroll()
{
        if (cursor_row < VGA_HEIGHT) {
                return;
        }

        for (size_t row = 1; row < VGA_HEIGHT; row++) {
                for (size_t col = 0; col < VGA_WIDTH; col++) {
                        VGA_MEMORY[(row - 1) * VGA_WIDTH + col] = VGA_MEMORY[row * VGA_WIDTH + col];
                }
        }

        for (size_t col = 0; col < VGA_WIDTH; col++) {
                VGA_MEMORY[(VGA_HEIGHT - 1) * VGA_WIDTH + col] = vga_entry(' ', KCOLOR);
        }

        cursor_row = VGA_HEIGHT - 1;
}

void kinit()
{
        cursor_row = 0;
        cursor_col = 0;
        KCOLOR = 0x0F;
        kclear();
}

void kclear()
{
        for (size_t row = 0; row < VGA_HEIGHT; row++) {
                for (size_t col = 0; col < VGA_WIDTH; col++) {
                        VGA_MEMORY[row * VGA_WIDTH + col] = vga_entry(' ', KCOLOR);
                }
        }
        cursor_row = 0;
        cursor_col = 0;
}

void tty_move_cursor(size_t row, size_t col) {
        if (row >= VGA_HEIGHT) {
                row = VGA_HEIGHT - 1;
        }
        if (col >= VGA_WIDTH) {
                col = VGA_WIDTH - 1;
        }

        cursor_row = row;
        cursor_col = col;

        uint16_t pos = cursor_row * VGA_WIDTH + cursor_col;

        outb(0x3D4, 0x0E);
        outb(0x3D5, (pos >> 8) & 0xFF);
        outb(0x3D4, 0x0F);
        outb(0x3D5, pos & 0xFF);
}

void tty_set_color(uint8_t color)
{
        KCOLOR = color;
}

void kputchar(char c)
{
        if (c == '\n') {
                cursor_col = 0;
                cursor_row++;
        } else {
                VGA_MEMORY[cursor_row * VGA_WIDTH + cursor_col] = vga_entry(c, KCOLOR);
                cursor_col++;
                if (cursor_col >= VGA_WIDTH) {
                        cursor_col = 0;
                        cursor_row++;
                }
        }

        if (cursor_row >= VGA_HEIGHT) {
                kscroll();
        }

        tty_move_cursor(cursor_row, cursor_col);
}

void int_to_str(int num, char *str)
{
        int i = 0;
        int is_negative = 0;

        if (num == 0) {
                str[i++] = '0';
                str[i] = '\0';
        }

        if (num < 0) {
                is_negative = 1;
                num = -num;
        }

        while (num != 0) {
                str[i++] = (num % 10) + '0';
                num /= 10;
        }

        if (is_negative) {
                str[i++] = '-';
    
        }

        str[i] = '\0';

        for (int start = 0, end = i - 1; start < end; start++, end--) {
                char tmp = str[start];
                str[start] = str[end];
                str[end] = tmp;
        }
}

void kprintf(const char *format, ...)
{
        va_list args;
        va_start(args, format);

        char buf[12];

        for (const char *p = format; *p != '\0'; p++) {
                if (*p == '%' && *(p+1) == '%') {
                        p += 2;
                        switch (*p) {
                                case 'g': 
                                        tty_set_color(GREEN_ON_BLACK);
                                        break;
                                case 'w': 
                                        tty_set_color(WHITE_ON_BLACK);
                                        break;
                                case 'y': 
                                        tty_set_color(YELLOW_ON_BLACK);
                                        break;
                                case 'r': 
                                        tty_set_color(RED_ON_BLACK);
                                        break;
                                case 'b': 
                                        tty_set_color(BLUE_ON_BLACK);
                                        break;
                                case 'l': 
                                        tty_set_color(LIGHT_GRAY_ON_BLACK);
                                        break;
                                default: 
                                        break;
                        }
                        continue;
                }

                if (*p == '%') {
                        p++;
                        switch (*p) {
                                case 'c': {
                                        char c = (char)va_arg(args, int);
                                        kputchar(c);
                                        break;
                                }
                                case 's': {
                                        const char *s = va_arg(args, const char *);
                                        for (int i = 0; s[i] != '\0'; i++) {
                                                kputchar(s[i]);
                                        }
                                        break;
                                }
                                case 'd': {
                                        int d = va_arg(args, int);
                                        int_to_str(d, buf);
                                        for (int i = 0; buf[i] != '\0'; i++) {
                                                kputchar(buf[i]);
                                        }
                                        break;
                                }
                                case 'u': {
                                        uint32_t u = va_arg(args, uint32_t);
                                        int_to_str(u, buf);
                                        for (int i = 0; buf[i] != '\0'; i++) {
                                                kputchar(buf[i]);
                                        }
                                        break;
                                }
                                case 'x': {
                                        uint32_t x = va_arg(args, uint32_t);
                                        const char *hex = "0123456789ABCDEF";
                                        for (int i = 28; i >= 0; i -= 4) {
                                                char ch = hex[(x >> i) & 0xF];
                                                kputchar(ch);
                                        }
                                        break;
                                }
                                case 'p': {
                                        void *ptr = va_arg(args, void *);
                                        size_t addr = (size_t)ptr;
                                        kputchar('0');
                                        kputchar('x');
                    
                                        const char *hex = "0123456789ABCDEF";
                                        for (int i = 60; i >= 0; i -= 4) {
                                                char ch = hex[(addr >> i) & 0xF];
                                                kputchar(ch);
                                        }
                                        break;
                                }
                                case '%':
                                        kputchar('%');
                                        break;
                                default:
                                        kputchar(*p);
                                        break;
                        }
                } else {
                        kputchar(*p);
                }
        }

        va_end(args);
}

__attribute__((noreturn))
void panic(const char *format, ...)
{

        tty_set_color(RED_ON_BLACK);

        va_list args;
        va_start(args, format);

        char buf[12];

        for (const char *p = format; *p != '\0'; p++) {
                if (*p == '%') {
                        p++;
                        char buf[12];
                        switch (*p) {
                                case 'c':
                                        kputchar((char)va_arg(args, int));
                                        break;
                                case 's':
                                        const char *s = va_arg(args, const char *);
                                        for (int i = 0; s[i]; i++) {
                                                kputchar(s[i]);
                                        }
                                        break;
                                case 'd':
                                        int_to_str(va_arg(args, int), buf);
                                        for (int i = 0; buf[i]; i++) {
                                                kputchar(buf[i]);
                                        }
                                        break;
                                case 'u':
                                        int_to_str(va_arg(args, uint32_t), buf);
                                        for (int i = 0; buf[i]; i++) {
                                                kputchar(buf[i]);
                                        }
                                        break;
                                case 'x':
                                        uint32_t x = va_arg(args, uint32_t);
                                        const char *hex = "0123456789ABCDEF";
                                        for (int i = 28; i >= 0; i -= 4) {
                                                kputchar(hex[(x >> i) & 0xF]);
                                        }
                                        break;
                                case '%': 
                                        kputchar('%');
                                        break;
                                default:
                                        kputchar(*p);
                                        break;
                        }
                } else {
                        kputchar(*p);
                }
        }

        va_end(args);


        tty_set_color(WHITE_ON_BLACK);
        
        while (1) {
                __asm__ __volatile__ ("hlt");
        }
}
