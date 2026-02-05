#ifndef __VGA_H__
#define __VGA_H__

#include <stdint.h>

/* VGA color modes */
typedef enum {
    VGA_COLOR_BLACK = 0,
    VGA_COLOR_BLUE = 1,
    VGA_COLOR_GREEN = 2,
    VGA_COLOR_CYAN = 3,
    VGA_COLOR_RED = 4,
    VGA_COLOR_MAGENTA = 5,
    VGA_COLOR_BROWN = 6,
    VGA_COLOR_LIGHT_GREY = 7,
    VGA_COLOR_DARK_GREY = 8,
    VGA_COLOR_LIGHT_BLUE = 9,
    VGA_COLOR_LIGHT_GREEN = 10,
    VGA_COLOR_LIGHT_CYAN = 11,
    VGA_COLOR_LIGHT_RED = 12,
    VGA_COLOR_LIGHT_MAGENTA = 13,
    VGA_COLOR_LIGHT_BROWN = 14,
    VGA_COLOR_WHITE = 15,
} vga_color_t;

#ifndef VGA_COLOR_LIGHT_YELLOW
#define VGA_COLOR_LIGHT_YELLOW VGA_COLOR_LIGHT_BROWN
#endif

typedef struct {
    uint16_t *buffer;
    uint32_t width;
    uint32_t height;
    uint32_t row;
    uint32_t col;
    vga_color_t fg_color;
    vga_color_t bg_color;
} vga_terminal_t;

/* Terminal functions */
void vga_initilize(vga_terminal_t *term, uint16_t *buffer, uint32_t width, uint32_t height);
void vga_clear_screen(vga_terminal_t *term);
void vga_putchar(vga_terminal_t *term, char c);
void vga_print(vga_terminal_t *term, const char *str);
void vga_print_hex(vga_terminal_t *term, uint64_t value);
void vga_println(vga_terminal_t *term, const char *str);
void vga_set_color(vga_terminal_t *term, vga_color_t fg, vga_color_t bg);

#endif /* __VGA_H__ */
