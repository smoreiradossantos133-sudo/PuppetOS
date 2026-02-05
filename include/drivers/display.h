/*
 * Graphics/Display Driver
 * Handles framebuffer rendering and display output
 */

#ifndef DISPLAY_H
#define DISPLAY_H

#include <kernel/kernel.h>

/* ===== COLOR FORMATS ===== */
typedef enum {
    COLOR_FORMAT_RGB888,    /* 24-bit RGB */
    COLOR_FORMAT_ARGB8888,  /* 32-bit ARGB */
    COLOR_FORMAT_BGR888,    /* 24-bit BGR */
    COLOR_FORMAT_BGRA8888,  /* 32-bit BGRA */
} color_format_t;

/* ===== COLOR ===== */
typedef uint32_t color_t;

/* Color constants */
#define COLOR_BLACK     0x00000000
#define COLOR_WHITE     0xFFFFFFFF
#define COLOR_RED       0xFF0000FF
#define COLOR_GREEN     0xFF00FF00
#define COLOR_BLUE      0xFFFF0000
#define COLOR_YELLOW    0xFF00FFFF
#define COLOR_CYAN      0xFFFFFF00
#define COLOR_MAGENTA   0xFFFF00FF

/* Windows 7 colors */
#define COLOR_WIN7_BLUE 0xFF539BD5
#define COLOR_WIN7_GRAY 0xFF232323
#define COLOR_WIN7_TASKBAR 0xFF1F1F1F

/* ===== DISPLAY INFORMATION ===== */
typedef struct {
    uint32_t width;
    uint32_t height;
    uint32_t pitch;       /* Bytes per scanline */
    uint32_t bpp;         /* Bits per pixel */
    vaddr_t framebuffer;
    color_format_t format;
} display_info_t;

/* ===== GRAPHICS CONTEXT ===== */
typedef struct {
    display_info_t info;
    color_t foreground;
    color_t background;
    uint32_t current_x;
    uint32_t current_y;
} graphics_context_t;

/* ===== GRAPHICS FUNCTIONS ===== */
void graphics_init(void);
void graphics_clear(color_t color);
void graphics_draw_pixel(uint32_t x, uint32_t y, color_t color);
void graphics_draw_rect(uint32_t x, uint32_t y, uint32_t width, uint32_t height, color_t color);
void graphics_fill_rect(uint32_t x, uint32_t y, uint32_t width, uint32_t height, color_t color);
void graphics_draw_line(uint32_t x1, uint32_t y1, uint32_t x2, uint32_t y2, color_t color);
void graphics_draw_circle(uint32_t x, uint32_t y, uint32_t radius, color_t color);

/* ===== TEXT RENDERING ===== */
void graphics_draw_char(uint32_t x, uint32_t y, char c, color_t color, color_t bg);
void graphics_draw_string(uint32_t x, uint32_t y, const char *str, color_t color, color_t bg);

/* ===== BLITTING ===== */
void graphics_blit(uint32_t x, uint32_t y, uint32_t width, uint32_t height, 
                   const color_t *data);

/* ===== UTILITIES ===== */
color_t graphics_make_color(uint8_t r, uint8_t g, uint8_t b);
color_t graphics_make_color_alpha(uint8_t r, uint8_t g, uint8_t b, uint8_t a);

#endif /* DISPLAY_H */
