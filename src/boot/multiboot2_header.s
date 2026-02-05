; PuppetOS - Multiboot2 Compatible Bootloader
; Implements Multiboot2 specification for loading kernel
; Can be used with QEMU's -kernel option or in custom bootloader chain

bits 32
section .multiboot_header
align 8

; Multiboot2 header for bootloader
MULTIBOOT2_MAGIC = 0xe85250d6
MULTIBOOT_ARCH = 0  ; i386

multiboot_header:
    dd MULTIBOOT2_MAGIC
    dd MULTIBOOT_ARCH
    dd (header_end - multiboot_header)
    dd -(MULTIBOOT2_MAGIC + MULTIBOOT_ARCH + (header_end - multiboot_header))
    
    ; Framebuffer tag
    align 8
tag_framebuffer:
    dw 5        ; Type: framebuffer
    dw 0        ; Flags: optional
    dd 20       ; Size
    dd 1024     ; Width
    dd 768      ; Height
    dd 32       ; Bits per pixel
    
    ; End tag
    align 8
    dw 0
    dw 0
    dd 8

header_end:

section .text
bits 32
global bootstrap

; Bootstrap code that sets up for kernel entry
bootstrap:
    ; This would be called by a parent bootloader
    ; For now, just a marker
    mov eax, 0x36d76289     ; Multiboot2 magic
    mov ebx, 0              ; Info pointer (would be set by bootloader)
    
    ; Call kernel main
    call kernel_main
    
    hlt
    jmp $

external kernel_main
