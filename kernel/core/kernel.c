/*
 * Kernel Core Implementation
 * Central kernel initialization and management
 */

#include <kernel/kernel.h>
#include <stddef.h>
#include <stdarg.h>
#include <vga.h>

/* ===== KERNEL STATE ===== */
kernel_state_t kernel_state = KERNEL_STATE_BOOTING;
struct boot_info bootinfo = {0};

static vga_terminal_t kernel_log_terminal;
static spinlock_t kernel_log_lock;

/* ===== INITIALIZATION ===== */
void kernel_init(void *limine_bootloader_info) {
    spinlock_init(&kernel_log_lock);
    
    /* Initialize VGA logging */
    uint16_t *vga_buffer = (uint16_t *)0xB8000;
    vga_initilize(&kernel_log_terminal, vga_buffer, 80, 25);
    vga_clear_screen(&kernel_log_terminal);
    
    /* Print boot banner */
    vga_set_color(&kernel_log_terminal, VGA_COLOR_LIGHT_CYAN, VGA_COLOR_BLACK);
    vga_println(&kernel_log_terminal, "╔════════════════════════════════════════╗");
    vga_println(&kernel_log_terminal, "║     PuppetOS Kernel v0.4.0              ║");
    vga_println(&kernel_log_terminal, "║     64-bit Operating System             ║");
    vga_println(&kernel_log_terminal, "║     Modern x86-64 Architecture          ║");
    vga_println(&kernel_log_terminal, "╚════════════════════════════════════════╝");
    vga_set_color(&kernel_log_terminal, VGA_COLOR_WHITE, VGA_COLOR_BLACK);
    vga_println(&kernel_log_terminal, "");
    
    KINFO("Kernel initialization starting...");
    KINFO("Bootloader: %s", limine_bootloader_info ? "Limine" : "Unknown");
    KINFO("CPU: x86-64 (AMD64)");
    KINFO("Boot time: %s %s", PUPPETOS_BUILD_DATE, PUPPETOS_BUILD_TIME);
    
    kernel_state = KERNEL_STATE_RUNNING;
    
    KINFO("Kernel ready!");
}

/* ===== LOGGING ===== */
void kernel_log(const char *level, const char *format, ...) {
    spinlock_acquire(&kernel_log_lock);
    
    va_list args;
    va_start(args, format);
    
    vga_set_color(&kernel_log_terminal, VGA_COLOR_LIGHT_GREEN, VGA_COLOR_BLACK);
    vga_print(&kernel_log_terminal, "[");
    vga_print(&kernel_log_terminal, level);
    vga_print(&kernel_log_terminal, "] ");
    vga_set_color(&kernel_log_terminal, VGA_COLOR_WHITE, VGA_COLOR_BLACK);
    
    // Simple format string handling
    const char *p = format;
    while (*p) {
        if (*p == '%' && *(p + 1)) {
            switch (*(p + 1)) {
                case 'd': {
                    int val = va_arg(args, int);
                    char buf[32];
                    int len = 0;
                    if (val < 0) {
                        buf[len++] = '-';
                        val = -val;
                    }
                    do {
                        buf[len++] = '0' + (val % 10);
                        val /= 10;
                    } while (val > 0);
                    for (int i = len - 1; i >= (buf[0] == '-' ? 1 : 0); i--) {
                        vga_print(&kernel_log_terminal, (char[]){buf[i], 0});
                    }
                    if (buf[0] == '-') vga_print(&kernel_log_terminal, "-");
                    p += 2;
                    break;
                }
                case 's': {
                    const char *str = va_arg(args, const char *);
                    vga_print(&kernel_log_terminal, (char *)str);
                    p += 2;
                    break;
                }
                case 'x': {
                    uint32_t val = va_arg(args, uint32_t);
                    vga_print(&kernel_log_terminal, "0x");
                    for (int i = 7; i >= 0; i--) {
                        uint8_t nibble = (val >> (i * 4)) & 0xF;
                        vga_print(&kernel_log_terminal, (char[]){
                            "0123456789ABCDEF"[nibble], 0
                        });
                    }
                    p += 2;
                    break;
                }
                default:
                    vga_print(&kernel_log_terminal, "%");
                    p++;
                    break;
            }
        } else {
            vga_print(&kernel_log_terminal, (char[]){*p, 0});
            p++;
        }
    }
    
    vga_println(&kernel_log_terminal, "");
    
    va_end(args);
    spinlock_release(&kernel_log_lock);
}

void kernel_warn(const char *format, ...) {
    spinlock_acquire(&kernel_log_lock);
    
    vga_set_color(&kernel_log_terminal, VGA_COLOR_LIGHT_YELLOW, VGA_COLOR_BLACK);
    vga_print(&kernel_log_terminal, "[WARN] ");
    vga_set_color(&kernel_log_terminal, VGA_COLOR_WHITE, VGA_COLOR_BLACK);
    
    // Format and print (simplified)
    vga_println(&kernel_log_terminal, (char *)format);
    
    spinlock_release(&kernel_log_lock);
}

void kernel_panic(const char *format, ...) {
    asm volatile("cli");  /* Disable interrupts */
    
    spinlock_acquire(&kernel_log_lock);
    vga_set_color(&kernel_log_terminal, VGA_COLOR_WHITE, VGA_COLOR_RED);
    vga_clear_screen(&kernel_log_terminal);
    vga_println(&kernel_log_terminal, "");
    vga_println(&kernel_log_terminal, "╔════════════════════════════════════════╗");
    vga_println(&kernel_log_terminal, "║           KERNEL PANIC                  ║");
    vga_println(&kernel_log_terminal, "╚════════════════════════════════════════╝");
    vga_println(&kernel_log_terminal, "");
    
    // Print panic message (simplified)
    vga_println(&kernel_log_terminal, (char *)format);
    vga_println(&kernel_log_terminal, "");
    vga_println(&kernel_log_terminal, "System halted.");
    
    kernel_state = KERNEL_STATE_PANIC;
    spinlock_release(&kernel_log_lock);
    
    while (1) {
        asm volatile("hlt");
    }
}
