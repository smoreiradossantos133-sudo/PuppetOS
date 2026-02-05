/*
 * PuppetOS Kernel - Limine Boot Version
 * Graphical Windows 7-inspired interface
 */

#include <stdint.h>
#include <stddef.h>
#include <drivers/display.h>
#include <drivers/input.h>
#include <ui/wm.h>

/* Apps */
extern void terminal_app_init(void);
extern void terminal_app_update(void);
extern void explorer_app_init(void);
extern void explorer_app_update(void);

/* Limine Protocol Structures */
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

struct limine_boot_info {
    uint64_t bootloader_brand;
    uint64_t bootloader_version;
    struct limine_framebuffer_response *framebuffer;
};

/* VGA Fallback */
static void vga_write(const char *msg) {
    uint16_t *vga = (uint16_t *)0xB8000;
    static int pos = 0;
    
    for (int i = 0; msg[i] && pos < 80 * 25; i++) {
        vga[pos++] = 0x0A00 | msg[i];
    }
}

void kernel_main_limine(void *boot_info_ptr) {
    /* Initialize graphics to VGA text as fallback */
    vga_write("PuppetOS v1.0 Boot...");
    
    if (!boot_info_ptr) {
        vga_write(" No bootloader info!");
        for (;;) __asm__("hlt");
    }
    
    struct limine_boot_info *info = (struct limine_boot_info *)boot_info_ptr;
    
    /* Check for framebuffer from Limine */
    if (info->framebuffer && info->framebuffer->framebuffer_count > 0) {
        struct limine_framebuffer *fb = info->framebuffer->framebuffers[0];
        
        vga_write(" Graphics OK! Initializing subsystems...");

        /* Initialize display driver (reads bootinfo internally) */
        graphics_init();

        /* Initialize window manager and input */
        wm_init();
        input_init();

        /* Start default apps */
        terminal_app_init();
        explorer_app_init();

        /* Main loop: process input, update WM and apps, render */
        for (;;) {
            /* Process pending input events */
            input_process_events();

            /* Update apps and window manager */
            terminal_app_update();
            explorer_app_update();
            wm_update();

            /* Render to screen when needed */
            wm_render();

            /* Yield */
            __asm__("hlt");
        }
    } else {
        vga_write(" No framebuffer!");
        for (;;) __asm__("hlt");
    }
}
