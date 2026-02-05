/* Linker script for PuppetOS 64-bit kernel */

ENTRY(_start)

SECTIONS {
    /* Kernel starts at 1MB (0x100000) */
    . = 0xFFFFFFFF80100000;
    
    /* Multiboot header section */
    .multiboot ALIGN(CONSTANT(COMMONPAGESIZE)) : AT(ADDR(.multiboot) - 0xFFFFFFFF80000000) {
        KEEP(*(.multiboot_header))
    }
    
    /* Code section */
    .text ALIGN(CONSTANT(COMMONPAGESIZE)) : AT(ADDR(.text) - 0xFFFFFFFF80000000) {
        *(.text)
        *(.text.*)
    }
    
    /* Read-only data */
    .rodata ALIGN(CONSTANT(COMMONPAGESIZE)) : AT(ADDR(.rodata) - 0xFFFFFFFF80000000) {
        *(.rodata)
        *(.rodata.*)
    }
    
    /* Initialized data */
    .data ALIGN(CONSTANT(COMMONPAGESIZE)) : AT(ADDR(.data) - 0xFFFFFFFF80000000) {
        *(.data)
        *(.data.*)
    }
    
    /* Uninitialized data (BSS) */
    .bss ALIGN(CONSTANT(COMMONPAGESIZE)) : AT(ADDR(.bss) - 0xFFFFFFFF80000000) {
        *(COMMON)
        *(.bss)
        *(.bss.*)
    }
    
    /* Remove unused sections */
    /DISCARD/ : {
        *(.note.GNU-stack)
        *(.gnu_debuglink)
        *(.gnu_debugdata)
    }
}
