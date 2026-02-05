bits 32

; Multiboot2 Header - Compatible with GRUB2
MULTIBOOT2_MAGIC equ 0xe85250d6
MULTIBOOT_ARCH equ 0

section .multiboot_header
align 8
multiboot_header:
    dd MULTIBOOT2_MAGIC
    dd MULTIBOOT_ARCH
    dd (header_end - multiboot_header)
    dd -(MULTIBOOT2_MAGIC + MULTIBOOT_ARCH + (header_end - multiboot_header))
    
    align 8
tag_end:
    dw 0
    dw 0
    dd 8
header_end:

section .text
extern kernel_main_limine
global _start

_start:
    cli
    cld
    
    ; Write to VGA text buffer to show boot message
    mov eax, 0xB8000
    mov byte [eax], 'B'
    mov byte [eax + 1], 0x02
    mov byte [eax + 2], 'T'
    mov byte [eax + 3], 0x02
    mov byte [eax + 4], '!'
    mov byte [eax + 5], 0x02
    
    mov esp, kernel_stack_top
    
    call kernel_main_limine
    
    cli
.halt:
    hlt
    jmp .halt

section .bss
align 16
kernel_stack:
    resq 1024
kernel_stack_top:
