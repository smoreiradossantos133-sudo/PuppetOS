#include <vga.h>
#include <types.h>

void vga_initilize(vga_terminal_t *term, uint16_t *buffer, uint32_t width, uint32_t height) {
    term->buffer = buffer;
    term->width = width;
    term->height = height;
    term->row = 0;
    term->col = 0;
    term->fg_color = VGA_COLOR_WHITE;
    term->bg_color = VGA_COLOR_BLACK;
    
    vga_clear_screen(term);
}

void vga_clear_screen(vga_terminal_t *term) {
    uint16_t blank = (term->bg_color << 12) | (term->fg_color << 8) | ' ';
    
    for (uint32_t i = 0; i < term->width * term->height; i++) {
        term->buffer[i] = blank;
    }
    
    term->row = 0;
    term->col = 0;
}

static void vga_scroll(vga_terminal_t *term) {
    uint16_t blank = (term->bg_color << 12) | (term->fg_color << 8) | ' ';
    
    /* Shift lines up */
    for (uint32_t i = 0; i < (term->height - 1) * term->width; i++) {
        term->buffer[i] = term->buffer[i + term->width];
    }
    
    /* Clear last line */
    for (uint32_t i = (term->height - 1) * term->width; i < term->height * term->width; i++) {
        term->buffer[i] = blank;
    }
    
    term->row = term->height - 1;
    term->col = 0;
}

void vga_putchar(vga_terminal_t *term, char c) {
    if (c == '\n') {
        term->col = 0;
        term->row++;
        if (term->row >= term->height) {
            vga_scroll(term);
        }
        return;
    }
    
    if (c == '\t') {
        term->col += 4;
        if (term->col >= term->width) {
            term->col = 0;
            term->row++;
            if (term->row >= term->height) {
                vga_scroll(term);
            }
        }
        return;
    }
    
    if (c == '\r') {
        term->col = 0;
        return;
    }
    
    uint32_t index = term->row * term->width + term->col;
    uint16_t entry = (term->bg_color << 12) | (term->fg_color << 8) | (uint8_t)c;
    term->buffer[index] = entry;
    
    term->col++;
    if (term->col >= term->width) {
        term->col = 0;
        term->row++;
        if (term->row >= term->height) {
            vga_scroll(term);
        }
    }
}

void vga_print(vga_terminal_t *term, const char *str) {
    while (*str) {
        vga_putchar(term, *str);
        str++;
    }
}

void vga_println(vga_terminal_t *term, const char *str) {
    vga_print(term, str);
    vga_putchar(term, '\n');
}

void vga_print_hex(vga_terminal_t *term, uint64_t value) {
    const char *hex = "0123456789ABCDEF";
    char buffer[17];
    int i = 15;
    buffer[16] = 0;
    
    if (value == 0) {
        vga_print(term, "0");
        return;
    }
    
    while (value > 0 && i >= 0) {
        buffer[i] = hex[value & 0xF];
        value >>= 4;
        i--;
    }
    
    vga_print(term, "0x");
    vga_print(term, &buffer[i + 1]);
}

void vga_set_color(vga_terminal_t *term, vga_color_t fg, vga_color_t bg) {
    term->fg_color = fg;
    term->bg_color = bg;
}
