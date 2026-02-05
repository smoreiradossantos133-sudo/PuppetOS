/*
 * Graphics Driver Implementation
 * Framebuffer rendering and display output
 */

#include <drivers/display.h>
#include <kernel/kernel.h>
#include <string.h>

/* ===== GRAPHICS CONTEXT ===== */
static graphics_context_t gfx_ctx = {0};
static bool graphics_initialized = false;

/* ===== SIMPLE FONT DATA (4x6 monospace) ===== */
static const uint8_t simple_font[256][6] = {
    /* Placeholder: Simple dot pattern for characters */
    [' '] = {0x00, 0x00, 0x00, 0x00, 0x00, 0x00},
    ['A'] = {0x7E, 0x11, 0x11, 0x7E, 0x00, 0x00},
    ['B'] = {0x7F, 0x49, 0x49, 0x36, 0x00, 0x00},
    /* ... more characters would go here ... */
};

/* ===== INITIALIZATION ===== */
void graphics_init(void) {
    if (graphics_initialized) return;
    
    /* Get framebuffer from bootloader info */
    gfx_ctx.info.width = bootinfo.framebuffer_width;
    gfx_ctx.info.height = bootinfo.framebuffer_height;
    gfx_ctx.info.pitch = bootinfo.framebuffer_pitch;
    gfx_ctx.info.bpp = bootinfo.framebuffer_bpp;
    gfx_ctx.info.framebuffer = bootinfo.framebuffer;
    gfx_ctx.info.format = COLOR_FORMAT_BGRA8888;
    
    gfx_ctx.foreground = COLOR_WHITE;
    gfx_ctx.background = COLOR_BLACK;
    gfx_ctx.current_x = 0;
    gfx_ctx.current_y = 0;
    
    graphics_initialized = true;
    
    KINFO("Graphics initialized: %dx%d, %d bpp", 
          gfx_ctx.info.width, gfx_ctx.info.height, gfx_ctx.info.bpp);
}

/* ===== BASIC DRAWING ===== */
void graphics_clear(color_t color) {
    if (!graphics_initialized) return;
    
    color_t *fb = (color_t *)gfx_ctx.info.framebuffer;
    uint32_t pixel_count = gfx_ctx.info.width * gfx_ctx.info.height;
    
    for (uint32_t i = 0; i < pixel_count; i++) {
        fb[i] = color;
    }
}

void graphics_draw_pixel(uint32_t x, uint32_t y, color_t color) {
    if (!graphics_initialized) return;
    if (x >= gfx_ctx.info.width || y >= gfx_ctx.info.height) return;
    
    color_t *fb = (color_t *)gfx_ctx.info.framebuffer;
    fb[y * gfx_ctx.info.width + x] = color;
}

void graphics_draw_rect(uint32_t x, uint32_t y, uint32_t width, uint32_t height, 
                       color_t color) {
    if (!graphics_initialized) return;
    
    /* Draw outline */
    for (uint32_t i = 0; i < width; i++) {
        graphics_draw_pixel(x + i, y, color);
        graphics_draw_pixel(x + i, y + height - 1, color);
    }
    
    for (uint32_t i = 0; i < height; i++) {
        graphics_draw_pixel(x, y + i, color);
        graphics_draw_pixel(x + width - 1, y + i, color);
    }
}

void graphics_fill_rect(uint32_t x, uint32_t y, uint32_t width, uint32_t height, 
                       color_t color) {
    if (!graphics_initialized) return;
    
    for (uint32_t yy = y; yy < y + height && yy < gfx_ctx.info.height; yy++) {
        for (uint32_t xx = x; xx < x + width && xx < gfx_ctx.info.width; xx++) {
            graphics_draw_pixel(xx, yy, color);
        }
    }
}

void graphics_draw_line(uint32_t x1, uint32_t y1, uint32_t x2, uint32_t y2, 
                       color_t color) {
    if (!graphics_initialized) return;
    
    /* Bresenham line algorithm */
    int dx = (x2 > x1) ? (x2 - x1) : (x1 - x2);
    int dy = (y2 > y1) ? (y2 - y1) : (y1 - y2);
    int sx = (x2 > x1) ? 1 : -1;
    int sy = (y2 > y1) ? 1 : -1;
    int err = dx - dy;
    
    int x = x1, y = y1;
    while (true) {
        graphics_draw_pixel(x, y, color);
        
        if (x == x2 && y == y2) break;
        
        int e2 = 2 * err;
        if (e2 > -dy) { err -= dy; x += sx; }
        if (e2 < dx) { err += dx; y += sy; }
    }
}

void graphics_draw_circle(uint32_t cx, uint32_t cy, uint32_t radius, color_t color) {
    if (!graphics_initialized) return;
    
    /* Midpoint circle algorithm */
    int x = radius;
    int y = 0;
    int err = 0;
    
    while (x >= y) {
        graphics_draw_pixel(cx + x, cy + y, color);
        graphics_draw_pixel(cx + y, cy + x, color);
        graphics_draw_pixel(cx - y, cy + x, color);
        graphics_draw_pixel(cx - x, cy + y, color);
        graphics_draw_pixel(cx - x, cy - y, color);
        graphics_draw_pixel(cx - y, cy - x, color);
        graphics_draw_pixel(cx + y, cy - x, color);
        graphics_draw_pixel(cx + x, cy - y, color);
        
        if (err <= 0) {
            y += 1;
            err += 2 * y + 1;
        } else {
            x -= 1;
            err -= 2 * x + 1;
        }
    }
}

/* ===== TEXT RENDERING ===== */
void graphics_draw_char(uint32_t x, uint32_t y, char c, color_t color, color_t bg) {
    if (!graphics_initialized) return;
    if ((uint8_t)c >= 256) return;
    
    // Draw background
    graphics_fill_rect(x, y, 4, 6, bg);
    
    // Draw character (simplified - would use font data)
    const uint8_t *glyph = simple_font[(uint8_t)c];
    
    // Draw pixel patterns (simplified)
    for (int i = 0; i < 6; i++) {
        for (int j = 0; j < 4; j++) {
            if ((glyph[i] >> j) & 1) {
                graphics_draw_pixel(x + j, y + i, color);
            }
        }
    }
}

void graphics_draw_string(uint32_t x, uint32_t y, const char *str, color_t color, 
                         color_t bg) {
    if (!graphics_initialized) return;
    
    uint32_t xpos = x;
    while (*str) {
        graphics_draw_char(xpos, y, *str, color, bg);
        xpos += 5;
        str++;
    }
}

/* ===== BLITTING ===== */
void graphics_blit(uint32_t x, uint32_t y, uint32_t width, uint32_t height, 
                  const color_t *data) {
    if (!graphics_initialized) return;
    
    color_t *fb = (color_t *)gfx_ctx.info.framebuffer;
    
    for (uint32_t yy = 0; yy < height && y + yy < gfx_ctx.info.height; yy++) {
        for (uint32_t xx = 0; xx < width && x + xx < gfx_ctx.info.width; xx++) {
            uint32_t src_idx = yy * width + xx;
            uint32_t dst_idx = (y + yy) * gfx_ctx.info.width + (x + xx);
            fb[dst_idx] = data[src_idx];
        }
    }
}

/* ===== COLOR UTILITIES ===== */
color_t graphics_make_color(uint8_t r, uint8_t g, uint8_t b) {
    return 0xFF000000 | (b << 16) | (g << 8) | r;
}

color_t graphics_make_color_alpha(uint8_t r, uint8_t g, uint8_t b, uint8_t a) {
    return (a << 24) | (b << 16) | (g << 8) | r;
}
