/*
 * Terminal Application
 * Simple command-line shell for PuppetOS
 */

#include <ui/wm.h>
#include <drivers/input.h>
#include <kernel/process.h>
#include <kernel/kernel.h>
#include <stddef.h>
#include <stdlib.h>
#include <string.h>

/* ===== TERMINAL STATE ===== */
typedef struct {
    window_t *window;
    char command_buffer[512];
    uint32_t buffer_pos;
    uint32_t cursor_x;
    uint32_t cursor_y;
    bool running;
} terminal_t;

static terminal_t terminal;

/* ===== BUILT-IN COMMANDS ===== */
static void cmd_help(void) {
    graphics_draw_string(terminal.window->x + 10, terminal.window->y + 40,
                        "Available commands:", COLOR_WHITE, terminal.window->background_color);
    graphics_draw_string(terminal.window->x + 10, terminal.window->y + 50,
                        "  help     - Show this help", COLOR_WHITE, terminal.window->background_color);
    graphics_draw_string(terminal.window->x + 10, terminal.window->y + 60,
                        "  clear    - Clear screen", COLOR_WHITE, terminal.window->background_color);
    graphics_draw_string(terminal.window->x + 10, terminal.window->y + 70,
                        "  ps       - List processes", COLOR_WHITE, terminal.window->background_color);
    graphics_draw_string(terminal.window->x + 10, terminal.window->y + 80,
                        "  exit     - Close terminal", COLOR_WHITE, terminal.window->background_color);
}

static void cmd_clear(void) {
    graphics_fill_rect(terminal.window->x + 1, terminal.window->y + 25,
                      terminal.window->width - 2, terminal.window->height - 26,
                      terminal.window->background_color);
    terminal.cursor_y = terminal.window->y + 30;
    terminal.cursor_x = terminal.window->x + 10;
}

static void cmd_ps(void) {
    graphics_draw_string(terminal.window->x + 10, terminal.cursor_y,
                        "Process List:", COLOR_WHITE, terminal.window->background_color);
    terminal.cursor_y += 15;
    
    process_list_all();
    
    graphics_draw_string(terminal.window->x + 10, terminal.cursor_y,
                        "  (See kernel log for details)", COLOR_YELLOW, terminal.window->background_color);
    terminal.cursor_y += 15;
}

static void cmd_echo(const char *args) {
    graphics_draw_string(terminal.cursor_x, terminal.cursor_y,
                        (char *)args, COLOR_WHITE, terminal.window->background_color);
    terminal.cursor_y += 15;
}

/* ===== COMMAND EXECUTION ===== */
static void terminal_execute_command(const char *cmd) {
    if (!cmd || cmd[0] == '\0') return;
    
    terminal.cursor_y += 15;
    
    /* Parse command */
    if (strcmp(cmd, "help") == 0) {
        cmd_help();
    } else if (strcmp(cmd, "clear") == 0) {
        cmd_clear();
    } else if (strcmp(cmd, "ps") == 0) {
        cmd_ps();
    } else if (strcmp(cmd, "exit") == 0) {
        terminal.running = false;
    } else if (strncmp(cmd, "echo ", 5) == 0) {
        cmd_echo(cmd + 5);
    } else {
        graphics_draw_string(terminal.cursor_x, terminal.cursor_y,
                            "Unknown command: ", COLOR_RED, terminal.window->background_color);
        graphics_draw_string(terminal.cursor_x + 130, terminal.cursor_y,
                            (char *)cmd, COLOR_RED, terminal.window->background_color);
        terminal.cursor_y += 15;
    }
    
    /* Draw prompt again */
    terminal.cursor_y += 5;
    graphics_draw_string(terminal.cursor_x, terminal.cursor_y,
                        ">>> ", COLOR_GREEN, terminal.window->background_color);
    terminal.cursor_x += 30;
}

/* ===== KEY INPUT HANDLER ===== */
static void terminal_key_handler(keycode_t key, bool pressed) {
    if (!pressed || !terminal.running) return;
    
    if (key == KEY_ENTER) {
        terminal_execute_command(terminal.command_buffer);
        terminal.command_buffer[0] = '\0';
        terminal.buffer_pos = 0;
    } else if (key == KEY_BACKSPACE && terminal.buffer_pos > 0) {
        terminal.buffer_pos--;
        terminal.command_buffer[terminal.buffer_pos] = '\0';
        terminal.window->needs_redraw = true;
    } else if (key >= KEY_A && key <= KEY_Z) {
        if (terminal.buffer_pos < sizeof(terminal.command_buffer) - 1) {
            char c = (char)('a' + (key - KEY_A));
            terminal.command_buffer[terminal.buffer_pos++] = c;
            terminal.command_buffer[terminal.buffer_pos] = '\0';
            terminal.window->needs_redraw = true;
        }
    } else if (key == KEY_SPACE) {
        if (terminal.buffer_pos < sizeof(terminal.command_buffer) - 1) {
            terminal.command_buffer[terminal.buffer_pos++] = ' ';
            terminal.command_buffer[terminal.buffer_pos] = '\0';
            terminal.window->needs_redraw = true;
        }
    }
}

/* ===== TERMINAL APPLICATION ===== */
void terminal_app_init(void) {
    KINFO("Terminal application starting...");
    
    /* Create terminal window */
    terminal.window = wm_create_window("PuppetOS Terminal", 50, 50, 800, 480, 0);
    if (!terminal.window) {
        KWARN("Failed to create terminal window");
        return;
    }
    
    wm_focus_window(terminal.window->window_id);
    
    terminal.command_buffer[0] = '\0';
    terminal.buffer_pos = 0;
    terminal.cursor_x = terminal.window->x + 10;
    terminal.cursor_y = terminal.window->y + 35;
    terminal.running = true;
    
    /* Register input handler */
    input_register_key_handler(terminal_key_handler);
    
    /* Draw initial prompt */
    graphics_fill_rect(terminal.window->x + 1, terminal.window->y + 25,
                      terminal.window->width - 2, terminal.window->height - 26,
                      terminal.window->background_color);
    
    graphics_draw_string(terminal.cursor_x, terminal.cursor_y,
                        ">>> ", COLOR_GREEN, terminal.window->background_color);
    terminal.cursor_x += 30;
    
    KINFO("Terminal ready!");
}

void terminal_app_update(void) {
    if (!terminal.running || !terminal.window) return;
    
    if (terminal.window->needs_redraw) {
        wm_draw_window(terminal.window);
        
        /* Redraw command prompt and buffer */
        graphics_draw_string(terminal.cursor_x, terminal.cursor_y,
                            terminal.command_buffer, COLOR_WHITE, 
                            terminal.window->background_color);
        
        terminal.window->needs_redraw = false;
    }
}
