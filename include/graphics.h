#ifndef __GRAPHICS_H__
#define __GRAPHICS_H__

#include <stdint.h>

void graphics_init(uint32_t *buffer, uint32_t width, uint32_t height, uint32_t pitch, uint16_t bpp);
void graphics_set_pixel(uint32_t x, uint32_t y, uint32_t color);
void graphics_fill_rect(uint32_t x, uint32_t y, uint32_t w, uint32_t h, uint32_t color);
void graphics_draw_rect_border(uint32_t x, uint32_t y, uint32_t w, uint32_t h, uint32_t color, uint32_t thickness);
void graphics_clear(uint32_t color);

/* Colors (BGR format) */
#define COLOR_BLACK       0x000000
#define COLOR_BLUE        0xFF0000
#define COLOR_GREEN       0x00FF00
#define COLOR_RED         0x0000FF
#define COLOR_WHITE       0xFFFFFF
#define COLOR_LIGHTBLUE   0xDEBB86  /* Windows 7 blur color */
#define COLOR_DARKGRAY    0x404040
#define COLOR_TASKBAR     0x1F1F1F  /* Taskbar dark gray */
#define COLOR_START_BTN   0x008000  /* Green start button */

#endif
