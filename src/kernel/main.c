#include <types.h>
#include <multiboot2.h>
#include <memory.h>
#include <vga.h>

/* Global terminal object */
static vga_terminal_t terminal;
static pmm_t pmm;

/* Kernel main function - called from boot.s */
void kernel_main(multiboot_tag_t *mbi, uint32_t magic) {
    /* Verify multiboot magic number */
    if (magic != MULTIBOOT2_BOOTLOADER_MAGIC) {
        __asm__("hlt");
        return;
    }
    
    /* Initialize VGA terminal at 0xB8000 (standard text mode buffer) */
    uint16_t *vga_buffer = (uint16_t *)0xB8000;
    vga_initilize(&terminal, vga_buffer, 80, 25);
    vga_clear_screen(&terminal);
    
    /* Print banner */
    vga_set_color(&terminal, VGA_COLOR_LIGHT_GREEN, VGA_COLOR_BLACK);
    vga_println(&terminal, "========================================");
    vga_println(&terminal, "       Welcome to PuppetOS 64-bit");
    vga_println(&terminal, "========================================");
    vga_println(&terminal, "");
    
    vga_set_color(&terminal, VGA_COLOR_WHITE, VGA_COLOR_BLACK);
    vga_println(&terminal, "Kernel Entry Point Reached!");
    vga_println(&terminal, "");
    
    /* Parse multiboot information */
    vga_println(&terminal, "Parsing Multiboot2 Information...");
    
    uint32_t total_memory = 0;
    multiboot_tag_t *tag = (multiboot_tag_t *)((uint8_t *)mbi + 8);
    
    while (tag->type != MULTIBOOT_TAG_TYPE_END) {
        if (tag->type == MULTIBOOT_TAG_TYPE_BASIC_MEMINFO) {
            multiboot_basic_meminfo_t *meminfo = (multiboot_basic_meminfo_t *)((uint8_t *)tag + 8);
            total_memory = meminfo->mem_upper;
            vga_print(&terminal, "  Memory: ");
            vga_print_hex(&terminal, (uint64_t)total_memory);
            vga_println(&terminal, " KB");
        }
        else if (tag->type == MULTIBOOT_TAG_TYPE_FRAMEBUFFER) {
            multiboot_framebuffer_tag_t *fb = (multiboot_framebuffer_tag_t *)tag;
            vga_print(&terminal, "  Framebuffer: ");
            vga_print_hex(&terminal, fb->framebuffer_addr);
            vga_print(&terminal, " (");
            vga_print_hex(&terminal, fb->framebuffer_width);
            vga_print(&terminal, "x");
            vga_print_hex(&terminal, fb->framebuffer_height);
            vga_println(&terminal, ")");
        }
        else if (tag->type == MULTIBOOT_TAG_TYPE_MMAP) {
            vga_println(&terminal, "  Memory Map found");
        }
        
        /* Move to next tag */
        tag = (multiboot_tag_t *)ALIGN_UP((uintptr_t)tag + tag->size, 8);
    }
    
    vga_println(&terminal, "");
    
    /* Initialize Physical Memory Manager */
    vga_println(&terminal, "Initializing Physical Memory Manager...");
    uint32_t total_frames = (total_memory * 1024) / PAGE_SIZE;
    pmm_init(&pmm, MIN(total_frames, PMM_MAX_FRAMES));
    vga_print(&terminal, "  Total Frames: ");
    vga_print_hex(&terminal, (uint64_t)pmm.num_frames);
    vga_println(&terminal, "");
    
    /* Test PMM allocation */
    vga_println(&terminal, "Testing PMM allocation...");
    uint32_t frame1 = pmm_alloc_frame(&pmm);
    uint32_t frame2 = pmm_alloc_frame(&pmm);
    uint32_t frame3 = pmm_alloc_frame(&pmm);
    vga_print(&terminal, "  Allocated frames: ");
    vga_print_hex(&terminal, frame1);
    vga_print(&terminal, ", ");
    vga_print_hex(&terminal, frame2);
    vga_print(&terminal, ", ");
    vga_print_hex(&terminal, frame3);
    vga_println(&terminal, "");
    vga_print(&terminal, "  Free frames: ");
    vga_print_hex(&terminal, (uint64_t)pmm_get_free_frames(&pmm));
    vga_println(&terminal, "");
    
    vga_println(&terminal, "");
    
    /* Initialize GDT */
    vga_println(&terminal, "Initializing GDT...");
    gdt_init();
    vga_println(&terminal, "  GDT initialized");
    
    /* Initialize IDT */
    vga_println(&terminal, "Initializing IDT...");
    idt_init();
    vga_println(&terminal, "  IDT initialized");
    
    /* Initialize Paging */
    vga_println(&terminal, "Initializing Paging...");
    paging_init();
    vga_println(&terminal, "  Paging initialized");
    
    vga_println(&terminal, "");
    
    vga_set_color(&terminal, VGA_COLOR_LIGHT_CYAN, VGA_COLOR_BLACK);
    vga_println(&terminal, "Kernel initialization complete!");
    vga_println(&terminal, "Entering kernel event loop...");
    vga_set_color(&terminal, VGA_COLOR_WHITE, VGA_COLOR_BLACK);
    
    /* Kernel event loop */
    while (1) {
        __asm__("hlt");  /* Wait for interrupt */
    }
}
