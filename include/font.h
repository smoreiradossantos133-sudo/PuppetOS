#ifndef __FONT_H__
#define __FONT_H__

#include <stdint.h>

void font_draw_char(uint32_t x, uint32_t y, char c, uint32_t fg_color, uint32_t bg_color);
void font_draw_string(uint32_t x, uint32_t y, const char *str, uint32_t fg_color, uint32_t bg_color);

#endif
