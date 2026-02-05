/*
 * Simple Explorer (File Manager) App
 */

#include <ui/wm.h>
#include <drivers/input.h>
#include <stddef.h>
#include <graphics.h>

typedef struct {
    window_t *window;
    bool running;
} explorer_t;

static explorer_t explorer;

static const char *mock_files[] = {
    "Program Files",
    "Users",
    "Windows",
    "README.txt",
    "config.sys",
};

void explorer_app_init(void) {
    KINFO("Explorer starting...");

    explorer.window = wm_create_window("Explorer", 120, 120, 500, 360, 0xFF202020);
    if (!explorer.window) {
        KWARN("Failed to create explorer window");
        return;
    }

    wm_focus_window(explorer.window->window_id);
    explorer.running = true;

    /* Draw initial content */
    graphics_fill_rect(explorer.window->x + 1, explorer.window->y + 25,
                       explorer.window->width - 2, explorer.window->height - 26,
                       explorer.window->background_color);

    int y = explorer.window->y + 35;
    for (size_t i = 0; i < sizeof(mock_files)/sizeof(mock_files[0]); ++i) {
        graphics_draw_string(explorer.window->x + 10, y, (char *)mock_files[i], COLOR_WHITE, explorer.window->background_color);
        y += 15;
    }

    KINFO("Explorer ready");
}

void explorer_app_update(void) {
    if (!explorer.running || !explorer.window) return;

    if (explorer.window->needs_redraw) {
        wm_draw_window(explorer.window);
        /* redraw file list */
        int y = explorer.window->y + 35;
        for (size_t i = 0; i < sizeof(mock_files)/sizeof(mock_files[0]); ++i) {
            graphics_draw_string(explorer.window->x + 10, y, (char *)mock_files[i], COLOR_WHITE, explorer.window->background_color);
            y += 15;
        }
        explorer.window->needs_redraw = false;
    }
}
