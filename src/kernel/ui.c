/* PuppetOS UI - Windows 7 Style Interface */
#include <graphics.h>
#include <input.h>
#include <font.h>

typedef struct {
    uint32_t x;
    uint32_t y;
    uint32_t width;
    uint32_t height;
    const char *title;
} window_t;

#define MAX_WINDOWS 16
static window_t windows[MAX_WINDOWS];
static uint32_t window_count = 0;

void ui_init(void) {
    window_count = 0;
}

void ui_draw_taskbar(void) {
    uint32_t taskbar_height = 48;
    uint32_t taskbar_y = 768 - taskbar_height;
    
    /* Taskbar background */
    graphics_fill_rect(0, taskbar_y, 1024, taskbar_height, COLOR_TASKBAR);
    
    /* Start button */
    graphics_fill_rect(5, taskbar_y + 5, 50, 38, COLOR_START_BTN);
    graphics_draw_rect_border(5, taskbar_y + 5, 50, 38, COLOR_WHITE, 1);
}

void ui_draw_desktop(void) {
    /* Windows 7 Aero blue background */
    graphics_clear(COLOR_LIGHTBLUE);
    
    /* Draw taskbar */
    ui_draw_taskbar();
}

void ui_draw_window(window_t *win) {
    /* Window background */
    graphics_fill_rect(win->x, win->y, win->width, win->height, 0xEBEBEB);
    
    /* Title bar */
    graphics_fill_rect(win->x, win->y, win->width, 30, 0x3E5C76);
    
    /* Window border */
    graphics_draw_rect_border(win->x, win->y, win->width, win->height, 0x808080, 2);
    
    /* Title text */
    if (win->title) {
        font_draw_string(win->x + 5, win->y + 7, win->title, COLOR_WHITE, 0x3E5C76);
    }
}

void ui_create_window(uint32_t x, uint32_t y, uint32_t w, uint32_t h, const char *title) {
    if (window_count >= MAX_WINDOWS) return;
    
    windows[window_count].x = x;
    windows[window_count].y = y;
    windows[window_count].width = w;
    windows[window_count].height = h;
    windows[window_count].title = title;
    
    window_count++;
}

void ui_render_all(void) {
    ui_draw_desktop();
    
    /* Draw all windows */
    for (uint32_t i = 0; i < window_count; i++) {
        ui_draw_window(&windows[i]);
    }
    
    /* Draw system info text */
    font_draw_string(20, 730, "PuppetOS v1.0", COLOR_WHITE, 0x1F1F1F);
    
    /* Draw mouse cursor */
    uint32_t mx = input_mouse_x();
    uint32_t my = input_mouse_y();
    
    /* Draw arrow cursor shape */
    graphics_fill_rect(mx, my, 3, 15, COLOR_WHITE);
    graphics_fill_rect(mx + 3, my + 3, 12, 3, COLOR_WHITE);
}
