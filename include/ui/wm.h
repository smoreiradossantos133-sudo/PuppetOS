/*
 * Window Manager
 * Manages windows, desktop, and UI rendering
 * Inspired by Windows 7 Aero interface
 */

#ifndef WINDOW_MANAGER_H
#define WINDOW_MANAGER_H

#include <kernel/kernel.h>
#include <drivers/display.h>
#include <drivers/input.h>

/* ===== WINDOW FLAGS ===== */
#define WINDOW_FLAG_VISIBLE    0x01
#define WINDOW_FLAG_FOCUSED    0x02
#define WINDOW_FLAG_RESIZABLE  0x04
#define WINDOW_FLAG_MINIMIZED  0x08
#define WINDOW_FLAG_MAXIMIZED  0x10
#define WINDOW_FLAG_FULLSCREEN 0x20
#define WINDOW_FLAG_MODAL      0x40

/* ===== WINDOW ===== */
typedef struct window {
    uint32_t window_id;
    
    char title[256];
    
    uint32_t x;
    uint32_t y;
    uint32_t width;
    uint32_t height;
    
    uint32_t flags;
    color_t background_color;
    
    kpid_t owner_pid;
    
    /* Window content (offscreen buffer) */
    color_t *framebuffer;
    uint32_t framebuffer_stride;
    
    /* Window state */
    bool needs_redraw;
    bool has_focus;
    
    /* Child windows */
    struct window **children;
    size_t num_children;
    
} window_t;

/* ===== DESKTOP ===== */
typedef struct {
    window_t **windows;
    size_t num_windows;
    window_t *focused_window;
    
    color_t background_color;
    char wallpaper_path[256];
    
    uint32_t screen_width;
    uint32_t screen_height;
} desktop_t;

/* ===== WINDOW MANAGER ===== */
void wm_init(void);
void wm_update(void);
void wm_render(void);

/* Window operations */
window_t *wm_create_window(const char *title, uint32_t x, uint32_t y, 
                           uint32_t width, uint32_t height, kpid_t owner);
void wm_destroy_window(uint32_t window_id);
void wm_show_window(uint32_t window_id);
void wm_hide_window(uint32_t window_id);
void wm_focus_window(uint32_t window_id);
void wm_move_window(uint32_t window_id, uint32_t x, uint32_t y);
void wm_resize_window(uint32_t window_id, uint32_t width, uint32_t height);
void wm_minimize_window(uint32_t window_id);
void wm_maximize_window(uint32_t window_id);
void wm_raise_window(uint32_t window_id);

/* Event handling */
void wm_handle_mouse_event(uint32_t x, uint32_t y, uint8_t buttons);
void wm_handle_key_event(keycode_t key, bool pressed);

/* Rendering */
void wm_draw_window(window_t *window);
void wm_draw_taskbar(void);
void wm_draw_desktop(void);

#endif /* WINDOW_MANAGER_H */
