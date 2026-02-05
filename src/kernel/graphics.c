/* PuppetOS Graphics Driver - Framebuffer Support */
#include <stdint.h>
#include <stddef.h>

typedef struct {
    uint8_t *buffer; /* raw byte pointer to framebuffer */
    uint32_t width;
    uint32_t height;
    uint32_t pitch; /* bytes per scanline */
    uint16_t bpp;
} framebuffer_t;

static framebuffer_t fb_global = {0};

/* Returns non-zero if framebuffer is initialized */
int graphics_present(void) {
    return fb_global.buffer != NULL && fb_global.width != 0 && fb_global.height != 0;
}

void graphics_init(uint32_t *buffer, uint32_t width, uint32_t height, uint32_t pitch, uint16_t bpp) {
    fb_global.buffer = (uint8_t *)buffer;
    fb_global.width = width;
    fb_global.height = height;
    fb_global.pitch = pitch; /* pitch already in bytes from bootloader */
    fb_global.bpp = bpp;
}

static inline void _put_pixel_raw(uint32_t x, uint32_t y, uint32_t color) {
    uint8_t bytes = fb_global.bpp / 8;
    uint8_t *dst = fb_global.buffer + y * fb_global.pitch + x * bytes;
    /* assume little-endian and 32-bit color (BGR/RGB as provided by bootloader) */
    if (bytes == 4) {
        *(uint32_t *)dst = color;
    } else if (bytes == 3) {
        dst[0] = (color & 0x0000FF);
        dst[1] = (color & 0x00FF00) >> 8;
        dst[2] = (color & 0xFF0000) >> 16;
    }
}

void graphics_set_pixel(uint32_t x, uint32_t y, uint32_t color) {
    if (!graphics_present()) return;
    if (x >= fb_global.width || y >= fb_global.height) return;
    _put_pixel_raw(x, y, color);
}

void graphics_fill_rect(uint32_t x, uint32_t y, uint32_t w, uint32_t h, uint32_t color) {
    if (!graphics_present()) return;
    if (x >= fb_global.width || y >= fb_global.height) return;
    uint32_t maxx = (x + w > fb_global.width) ? fb_global.width : x + w;
    uint32_t maxy = (y + h > fb_global.height) ? fb_global.height : y + h;
    for (uint32_t yy = y; yy < maxy; yy++) {
        for (uint32_t xx = x; xx < maxx; xx++) {
            _put_pixel_raw(xx, yy, color);
        }
    }
}

void graphics_draw_rect_border(uint32_t x, uint32_t y, uint32_t w, uint32_t h, uint32_t color, uint32_t thickness) {
    if (!graphics_present()) return;
    if (w == 0 || h == 0) return;
    if (thickness == 0) return;
    /* Top */
    graphics_fill_rect(x, y, w, thickness, color);
    /* Bottom */
    if (h > thickness) graphics_fill_rect(x, y + h - thickness, w, thickness, color);
    /* Left */
    graphics_fill_rect(x, y, thickness, h, color);
    /* Right */
    if (w > thickness) graphics_fill_rect(x + w - thickness, y, thickness, h, color);
}

void graphics_clear(uint32_t color) {
    graphics_fill_rect(0, 0, fb_global.width, fb_global.height, color);
}
