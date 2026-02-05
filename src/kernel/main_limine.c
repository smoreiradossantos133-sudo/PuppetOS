/*
 * PuppetOS Kernel - Limine Boot Version v0.3.0
 * Modern bootloader with graphics support
 * Inspired by Windows 7 interface
 */

#include <stdint.h>
#include <stddef.h>
#include <vga.h>

/* ====== LIMINE PROTOCOL STRUCTURES ====== */

struct limine_framebuffer {
    uint64_t address;
    uint64_t width;
    uint64_t height;
    uint64_t pitch;
    uint16_t bpp;
    uint8_t red_mask_size;
    uint8_t red_mask_shift;
    uint8_t green_mask_size;
    uint8_t green_mask_shift;
    uint8_t blue_mask_size;
    uint8_t blue_mask_shift;
};

struct limine_framebuffer_response {
    uint64_t revision;
    uint64_t framebuffer_count;
    struct limine_framebuffer **framebuffers;
};

struct limine_memmap_entry {
    uint64_t base;
    uint64_t length;
    uint32_t type;
    uint32_t unused;
};

struct limine_memmap_response {
    uint64_t revision;
    uint64_t entry_count;
    struct limine_memmap_entry **entries;
};

struct limine_boot_info {
    uint64_t bootloader_brand;
    uint64_t bootloader_version;
    struct limine_framebuffer_response *framebuffer;
    struct limine_memmap_response *memmap;
};

/* ====== KERNEL STATE ====== */

static struct limine_framebuffer *fb = NULL;
static uint32_t screen_width = 0;
static uint32_t screen_height = 0;
static uint32_t screen_pitch = 0;
static uint64_t total_memory = 0;
static uint64_t usable_memory = 0;

/* ====== GRAPHICS FUNCTIONS ====== */

static void draw_pixel(uint32_t x, uint32_t y, uint32_t color) {
    if (!fb || x >= screen_width || y >= screen_height) {
        return;
    }
    uint32_t *pixel = (uint32_t *)((uint8_t *)fb->address + y * screen_pitch + x * 4);
    *pixel = color;
}

static void fill_rect(uint32_t x, uint32_t y, uint32_t width, uint32_t height, uint32_t color) {
    for (uint32_t yy = y; yy < y + height && yy < screen_height; yy++) {
        for (uint32_t xx = x; xx < x + width && xx < screen_width; xx++) {
            draw_pixel(xx, yy, color);
        }
    }
}

static void draw_line(uint32_t x1, uint32_t y1, uint32_t x2, uint32_t y2, uint32_t color) {
    int dx = (x2 > x1) ? (x2 - x1) : (x1 - x2);
    int dy = (y2 > y1) ? (y2 - y1) : (y1 - y2);
    
    if (dx == 0) {
        for (uint32_t y = y1; y <= y2; y++) draw_pixel(x1, y, color);
    } else if (dy == 0) {
        for (uint32_t x = x1; x <= x2; x++) draw_pixel(x, y1, color);
    }
}

static void draw_border(uint32_t x, uint32_t y, uint32_t w, uint32_t h, uint32_t color) {
    draw_line(x, y, x + w, y, color);                    // Top
    draw_line(x + w, y, x + w, y + h, color);            // Right
    draw_line(x + w, y + h, x, y + h, color);            // Bottom
    draw_line(x, y + h, x, y, color);                    // Left
}

/* ====== UI DRAWING ====== */

static void draw_wallpaper(void) {
    if (!fb) return;
    
    // Windows 7 blue theme
    uint32_t win7_blue = 0x00539BD5;    // BGR format
    fill_rect(0, 0, screen_width, screen_height, win7_blue);
}

static void draw_taskbar(void) {
    if (!fb) return;
    
    uint32_t taskbar_height = 48;
    uint32_t taskbar_y = screen_height - taskbar_height;
    
    // Taskbar background (gradient effect - simplified)
    uint32_t taskbar_color = 0x00232328;  // Dark gray
    fill_rect(0, taskbar_y, screen_width, taskbar_height, taskbar_color);
    
    // Taskbar border (top edge highlight)
    fill_rect(0, taskbar_y - 2, screen_width, 2, 0x00464646);
}

static void draw_start_button(void) {
    if (!fb) return;
    
    uint32_t taskbar_y = screen_height - 48;
    uint32_t btn_x = 5;
    uint32_t btn_y = taskbar_y + 5;
    uint32_t btn_w = 50;
    uint32_t btn_h = 38;
    
    fill_rect(btn_x, btn_y, btn_w, btn_h, 0x00008000);  // Windows green
    draw_border(btn_x, btn_y, btn_w, btn_h, 0x00FFFFFF);
}

static void draw_system_clock(void) {
    if (!fb) return;
    
    // Draw clock area (simplified)
    uint32_t taskbar_y = screen_height - 48;
    uint32_t clock_w = 70;
    uint32_t clock_x = screen_width - clock_w - 5;
    uint32_t clock_y = taskbar_y + 10;
    
    fill_rect(clock_x, clock_y, clock_w, 30, 0x00464646);
    draw_border(clock_x, clock_y, clock_w, 30, 0x00808080);
}

/* ====== MEMORY MANAGEMENT ====== */

static void process_memmap(struct limine_memmap_response *memmap) {
    if (!memmap) return;
    
    for (uint64_t i = 0; i < memmap->entry_count; i++) {
        struct limine_memmap_entry *entry = memmap->entries[i];
        total_memory += entry->length;
        
        if (entry->type == 0) {  // LIMINE_MEMMAP_USABLE
            usable_memory += entry->length;
        }
    }
}

/* ====== KERNEL ENTRY POINT ====== */
// Forward declarations
extern void kernel_init(void *limine_bootloader_info);
extern void scheduler_init(void);
extern void graphics_init(void);
extern void wm_init(void);
extern void input_init(void);
extern void terminal_app_init(void);
extern void terminal_app_update(void);
extern void wm_update(void);
extern void wm_render(void);
extern void input_process_events(void);

void kernel_main_limine(void *limine_boot_info) {
    /* Write directly to VGA text buffer */
    uint16_t *vga_buffer = (uint16_t *)0xB8000;
    
    /* Clear screen */
    for (int i = 0; i < 80 * 25; i++) {
        vga_buffer[i] = 0x0020;
    }
    
    /* Write boot message */
    const char *msg = "PuppetOS Booted Successfully!";
    uint16_t color = 0x0A00;  /* Green on black */
    
    for (int i = 0; msg[i] != '\0'; i++) {
        vga_buffer[i] = color | msg[i];
    }
    
    /* Halt forever */
    for (;;) {
        __asm__("hlt");
    }
}

