#include "tty.h"
#include "string.h"
#include "io.h"
#include "xenon/arg.h"


unsigned int *framebuffer = (unsigned int *)0xB8000;

static __volatile__ uint16_t *VGA_MEMORY = (__volatile__ uint16_t*)0xB8000;

static int cursor_row;
static int cursor_col;
static uint8_t KCOLOR;

#define CURSOR_SIZE 10



static inline uint16_t vga_entry(unsigned char c, uint8_t color)
{
        return (uint16_t)c | ((uint16_t)color << 8);
}

int cursor_x = 0;
int cursor_y = 0;
int screen_width = 1024;
int screen_height = 768;


void tty_move_cursor(size_t row, size_t col);

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
                cursor_row = VGA_HEIGHT - 1;
        }

        tty_move_cursor(cursor_row, cursor_col);
}


void tty_set_color(uint8_t color)
{
        KCOLOR = color;
}

void tty_move_cursor(size_t row, size_t col)
{
        if (row >= VGA_HEIGHT || col >= VGA_WIDTH) {
                return;
        }

        cursor_row = row;
        cursor_col = col;

        unsigned short pos = cursor_row * VGA_WIDTH + cursor_col;

        outb(0x3D4, 0x0E);
        outb(0x3D5, (pos >> 8) & 0xFF);

        outb(0x3D4, 0x0F);
        outb(0x3D5, pos & 0xFF);
}


void int_to_str(int num, char *str)
{
        int i = 0;
        int is_negative = 0;

        if (num == 0) {
                str[i++] = '0';
                str[i] = '\0';
                return;
        }

        if (num < 0) {
                is_negative = 1;
                num = -num;
        }

        while (num != 0) {
                int digit = num % 10;
                str[i++] = digit + '0';
                num /= 10;
        }

        if (is_negative) {
                str[i++] = '-';
        }

        str[i] = '\0';

        int start = 0;
        int end = i - 1;
        while (start < end) {
                char tmp = str[start];
                str[start] = str[end];
                str[end] = tmp;
                start++;
                end--;
        }
}

void kprintf(const char *format, ...)
{
    va_list args;
    va_start(args, format);

    for (const char *p = format; *p != '\0'; p++) {
        if (*p == '%' && *(p+1) == '%') {
            p += 2;
            switch (*p) {
                case 'g': tty_set_color(GREEN_ON_BLACK); break;
                case 'w': tty_set_color(WHITE_ON_BLACK); break;
                case 'y': tty_set_color(YELLOW_ON_BLACK); break;
                case 'r': tty_set_color(RED_ON_BLACK); break;
                default: break;
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
                    char buffer[12];
                    int d = va_arg(args, int);
                    int_to_str(d, buffer);
                    for (int i = 0; buffer[i] != '\0'; i++) {
                        kputchar(buffer[i]);
                    }
                    break;
                }
                case 'u': {  // unsigned decimal
                    char buffer[12];
                    unsigned int u = va_arg(args, unsigned int);
                    int_to_str(u, buffer);
                    for (int i = 0; buffer[i] != '\0'; i++) {
                        kputchar(buffer[i]);
                    }
                    break;
                }
                case 'x': {
                    unsigned int x = va_arg(args, unsigned int);
                    const char *hex = "0123456789ABCDEF";
                    for (int i = 28; i >= 0; i -= 4) {
                        char ch = hex[(x >> i) & 0xF];
                        kputchar(ch);
                    }
                    break;
                }
                case 'p': {
                    void *ptr = va_arg(args, void *);
                    unsigned long addr = (unsigned long)ptr;
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

    for (const char *p = format; *p != '\0'; p++) {
        if (*p == '%') {
            p++;
            switch (*p) {
                case 'c': kputchar((char)va_arg(args, int)); break;
                case 's': {
                    const char *s = va_arg(args, const char *);
                    for (int i = 0; s[i]; i++) kputchar(s[i]);
                    break;
                }
                case 'd': {
                    char buf[12];
                    int_to_str(va_arg(args, int), buf);
                    for (int i = 0; buf[i]; i++) kputchar(buf[i]);
                    break;
                }
                case 'u': {
                    char buf[12];
                    int_to_str(va_arg(args, unsigned int), buf);
                    for (int i = 0; buf[i]; i++) kputchar(buf[i]);
                    break;
                }
                case 'x': {
                    unsigned int x = va_arg(args, unsigned int);
                    const char *hex = "0123456789ABCDEF";
                    for (int i = 28; i >= 0; i -= 4) kputchar(hex[(x >> i) & 0xF]);
                    break;
                }
                case 'p': {
                    unsigned long addr = (unsigned long)va_arg(args, void *);
                    kputchar('0'); kputchar('x');
                    const char *hex = "0123456789ABCDEF";
                    for (int i = 60; i >= 0; i -= 4) kputchar(hex[(addr >> i) & 0xF]);
                    break;
                }
                case '%': kputchar('%'); break;
                default: kputchar(*p); break;
            }
        } else {
            kputchar(*p);
        }
    }

    va_end(args);


        tty_set_color(WHITE_ON_BLACK);

    while (1) {
        __asm__ volatile("hlt");
    }
}
