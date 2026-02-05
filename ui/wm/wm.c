/*
 * Window Manager Implementation
 * Windows 7 Aero-inspired desktop environment
 */

#include <ui/wm.h>
#include <drivers/display.h>
#include <drivers/input.h>
#include <kernel/kernel.h>
#include <string.h>
#include <stdlib.h>

/* ===== DESKTOP STATE ===== */
static desktop_t desktop = {0};
static uint32_t next_window_id = 1;
static spinlock_t wm_lock;

/* ===== WINDOW MANAGEMENT ===== */
void wm_init(void) {
    spinlock_init(&wm_lock);
    
    desktop.windows = (window_t **)malloc(sizeof(window_t *) * 256);
    desktop.num_windows = 0;
    desktop.focused_window = NULL;
    
    desktop.background_color = COLOR_WIN7_BLUE;
    desktop.screen_width = bootinfo.framebuffer_width;
    desktop.screen_height = bootinfo.framebuffer_height;
    
    KINFO("Window Manager initialized (%dx%d)", desktop.screen_width, desktop.screen_height);
}

window_t *wm_create_window(const char *title, uint32_t x, uint32_t y, 
                           uint32_t width, uint32_t height, kpid_t owner) {
    spinlock_acquire(&wm_lock);
    
    if (desktop.num_windows >= 256) {
        spinlock_release(&wm_lock);
        return NULL;
    }
    
    window_t *window = (window_t *)malloc(sizeof(window_t));
    if (!window) {
        spinlock_release(&wm_lock);
        return NULL;
    }
    
    window->window_id = next_window_id++;
    strncpy(window->title, title, 255);
    window->title[255] = '\0';
    
    window->x = x;
    window->y = y;
    window->width = width;
    window->height = height;
    
    window->flags = WINDOW_FLAG_VISIBLE;
    window->background_color = COLOR_WIN7_GRAY;
    window->owner_pid = owner;
    
    window->framebuffer = (color_t *)malloc(width * height * sizeof(color_t));
    window->framebuffer_stride = width;
    
    window->needs_redraw = true;
    window->has_focus = false;
    window->children = NULL;
    window->num_children = 0;
    
    desktop.windows[desktop.num_windows++] = window;
    
    KINFO("Window created: %s (ID %d, %ux%u @ %u,%u)", 
          title, window->window_id, width, height, x, y);
    
    spinlock_release(&wm_lock);
    return window;
}

void wm_destroy_window(uint32_t window_id) {
    spinlock_acquire(&wm_lock);
    
    for (uint32_t i = 0; i < desktop.num_windows; i++) {
        if (desktop.windows[i]->window_id == window_id) {
            free(desktop.windows[i]->framebuffer);
            free(desktop.windows[i]);
            
            for (uint32_t j = i; j < desktop.num_windows - 1; j++) {
                desktop.windows[j] = desktop.windows[j + 1];
            }
            desktop.num_windows--;
            
            KINFO("Window destroyed (ID %d)", window_id);
            break;
        }
    }
    
    spinlock_release(&wm_lock);
}

void wm_show_window(uint32_t window_id) {
    spinlock_acquire(&wm_lock);
    
    for (uint32_t i = 0; i < desktop.num_windows; i++) {
        if (desktop.windows[i]->window_id == window_id) {
            desktop.windows[i]->flags |= WINDOW_FLAG_VISIBLE;
            desktop.windows[i]->needs_redraw = true;
            break;
        }
    }
    
    spinlock_release(&wm_lock);
}

void wm_hide_window(uint32_t window_id) {
    spinlock_acquire(&wm_lock);
    
    for (uint32_t i = 0; i < desktop.num_windows; i++) {
        if (desktop.windows[i]->window_id == window_id) {
            desktop.windows[i]->flags &= ~WINDOW_FLAG_VISIBLE;
            break;
        }
    }
    
    spinlock_release(&wm_lock);
}

void wm_focus_window(uint32_t window_id) {
    spinlock_acquire(&wm_lock);
    
    /* Remove focus from current window */
    if (desktop.focused_window) {
        desktop.focused_window->has_focus = false;
        desktop.focused_window->needs_redraw = true;
    }
    
    /* Set focus to new window */
    for (uint32_t i = 0; i < desktop.num_windows; i++) {
        if (desktop.windows[i]->window_id == window_id) {
            desktop.windows[i]->has_focus = true;
            desktop.windows[i]->needs_redraw = true;
            desktop.focused_window = desktop.windows[i];
            break;
        }
    }
    
    spinlock_release(&wm_lock);
}

void wm_move_window(uint32_t window_id, uint32_t x, uint32_t y) {
    spinlock_acquire(&wm_lock);
    
    for (uint32_t i = 0; i < desktop.num_windows; i++) {
        if (desktop.windows[i]->window_id == window_id) {
            desktop.windows[i]->x = x;
            desktop.windows[i]->y = y;
            desktop.windows[i]->needs_redraw = true;
            break;
        }
    }
    
    spinlock_release(&wm_lock);
}

void wm_resize_window(uint32_t window_id, uint32_t width, uint32_t height) {
    spinlock_acquire(&wm_lock);
    
    for (uint32_t i = 0; i < desktop.num_windows; i++) {
        if (desktop.windows[i]->window_id == window_id) {
            window_t *w = desktop.windows[i];
            
            /* Reallocate framebuffer */
            if (w->framebuffer) free(w->framebuffer);
            w->framebuffer = (color_t *)malloc(width * height * sizeof(color_t));
            w->framebuffer_stride = width;
            
            w->width = width;
            w->height = height;
            w->needs_redraw = true;
            break;
        }
    }
    
    spinlock_release(&wm_lock);
}

void wm_minimize_window(uint32_t window_id) {
    spinlock_acquire(&wm_lock);
    
    for (uint32_t i = 0; i < desktop.num_windows; i++) {
        if (desktop.windows[i]->window_id == window_id) {
            desktop.windows[i]->flags |= WINDOW_FLAG_MINIMIZED;
            break;
        }
    }
    
    spinlock_release(&wm_lock);
}

void wm_maximize_window(uint32_t window_id) {
    spinlock_acquire(&wm_lock);
    
    for (uint32_t i = 0; i < desktop.num_windows; i++) {
        if (desktop.windows[i]->window_id == window_id) {
            desktop.windows[i]->flags |= WINDOW_FLAG_MAXIMIZED;
            desktop.windows[i]->x = 0;
            desktop.windows[i]->y = 0;
            desktop.windows[i]->width = desktop.screen_width;
            desktop.windows[i]->height = desktop.screen_height - 48;  /* Reserve taskbar */
            break;
        }
    }
    
    spinlock_release(&wm_lock);
}

void wm_raise_window(uint32_t window_id) {
    spinlock_acquire(&wm_lock);
    
    /* Find window and move to end (rendered last = on top) */
    window_t *target = NULL;
    for (uint32_t i = 0; i < desktop.num_windows; i++) {
        if (desktop.windows[i]->window_id == window_id) {
            target = desktop.windows[i];
            
            for (uint32_t j = i; j < desktop.num_windows - 1; j++) {
                desktop.windows[j] = desktop.windows[j + 1];
            }
            desktop.windows[desktop.num_windows - 1] = target;
            break;
        }
    }
    
    spinlock_release(&wm_lock);
}

/* ===== RENDERING ===== */
void wm_draw_window(window_t *window) {
    if (!window || !(window->flags & WINDOW_FLAG_VISIBLE)) return;
    
    /* Draw window background */
    graphics_fill_rect(window->x, window->y, window->width, window->height, 
                      window->background_color);
    
    /* Draw title bar */
    color_t title_color = window->has_focus ? 0xFF0078D4 : 0xFF808080;
    graphics_fill_rect(window->x, window->y, window->width, 25, title_color);
    
    /* Draw title text */
    graphics_draw_string(window->x + 5, window->y + 5, window->title, 
                        COLOR_WHITE, title_color);
    
    /* Draw window border */
    graphics_draw_rect(window->x, window->y, window->width, window->height, 
                      window->has_focus ? COLOR_WHITE : 0xFF808080);
}

void wm_draw_taskbar(void) {
    uint32_t taskbar_y = desktop.screen_height - 48;
    
    /* Taskbar background */
    graphics_fill_rect(0, taskbar_y, desktop.screen_width, 48, COLOR_WIN7_TASKBAR);
    
    /* Start button */
    graphics_fill_rect(5, taskbar_y + 5, 50, 38, 0xFF008000);
    graphics_draw_rect(5, taskbar_y + 5, 50, 38, COLOR_WHITE);
    graphics_draw_string(10, taskbar_y + 12, "Start", COLOR_WHITE, 0xFF008000);
    
    /* Window buttons in taskbar (simplified) */
    uint32_t btn_x = 60;
    for (uint32_t i = 0; i < desktop.num_windows; i++) {
        if (desktop.windows[i]->flags & WINDOW_FLAG_VISIBLE &&
            !(desktop.windows[i]->flags & WINDOW_FLAG_MINIMIZED)) {
            
            graphics_fill_rect(btn_x, taskbar_y + 5, 150, 38, 0xFF464646);
            graphics_draw_rect(btn_x, taskbar_y + 5, 150, 38, 0xFF808080);
            graphics_draw_string(btn_x + 5, taskbar_y + 12, desktop.windows[i]->title, 
                              COLOR_WHITE, 0xFF464646);
            
            btn_x += 155;
            if (btn_x > desktop.screen_width - 100) break;
        }
    }
    
    /* System clock area */
    graphics_draw_rect(desktop.screen_width - 75, taskbar_y + 10, 70, 30, 0xFF808080);
}

void wm_draw_desktop(void) {
    /* Clear with wallpaper color */
    graphics_clear(desktop.background_color);
    
    /* Draw all windows (back to front) */
    for (uint32_t i = 0; i < desktop.num_windows; i++) {
        wm_draw_window(desktop.windows[i]);
    }
    
    /* Draw taskbar */
    wm_draw_taskbar();
}

void wm_update(void) {
    spinlock_acquire(&wm_lock);
    
    bool needs_redraw = false;
    for (uint32_t i = 0; i < desktop.num_windows; i++) {
        if (desktop.windows[i]->needs_redraw) {
            needs_redraw = true;
            desktop.windows[i]->needs_redraw = false;
        }
    }
    
    spinlock_release(&wm_lock);
    
    if (needs_redraw) {
        wm_render();
    }
}

void wm_render(void) {
    wm_draw_desktop();
}

void wm_handle_mouse_event(uint32_t x, uint32_t y, uint8_t buttons) {
    /* TODO: Hit-testing and window focus */
}

void wm_handle_key_event(keycode_t key, bool pressed) {
    /* TODO: Keyboard input routing */
}
