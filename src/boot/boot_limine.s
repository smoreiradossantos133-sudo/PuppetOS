bits 64

; Limine Boot Protocol Entry Point
; Limine bootloader provides complete 64-bit environment
; and passes boot information via RDI (bootloader info pointer)

section .text
global _start
extern kernel_main_limine

_start:
    ; Limine has already:
    ; ✅ Enabled Long Mode (64-bit)
    ; ✅ Set up paging with higher-half kernel mapping
    ; ✅ Loaded GDT/IDT
    ; ✅ Set up stack
    ; ✅ Initialized interrupts
    ;
    ; We just call kernel_main_limine!
    
    cli                         ; Disable interrupts
    cld                         ; Clear direction flag
    
    ; RDI contains pointer to bootloader info structure
    ; This is already set by Limine
    
    ; Call C kernel entry point
    call kernel_main_limine
    
    ; If kernel_main returns, halt forever
    cli
.halt:
    hlt
    jmp .halt

; Ensure proper section alignment for Limine
section .bss
align 16
kernel_stack:
    resq 1024              ; 8KB kernel stack
kernel_stack_top:

