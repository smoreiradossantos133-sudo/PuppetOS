#ifndef __UI_H__
#define __UI_H__

#include <stdint.h>

void ui_init(void);
void ui_draw_desktop(void);
void ui_create_window(uint32_t x, uint32_t y, uint32_t w, uint32_t h, const char *title);
void ui_render_all(void);

#endif
