bits 32

; Multiboot2 header  
section .multiboot_header
align 8

multiboot_header:
    dd 0xe85250d6
    dd 0
    dd (header_end - multiboot_header)
    dd -(0xe85250d6 + 0 + (header_end - multiboot_header))

    ; Framebuffer tag
    align 8
    dw 5
    dw 0
    dd 20
    dd 1024
    dd 768
    dd 32

    ; End tag
    align 8
    dw 0
    dw 0
    dd 8

header_end:

section .text
global _start
extern kernel_main

_start:
    ; GRUB v2 multiboot2
    ; EAX = magic
    ; EBX = info structure
    
    cli
    mov esp, 0x90000
    
    ; Debug: Write 'B' to VGA at 0xB8000 (first character)
    mov byte [0xB8000], 'B'
    mov byte [0xB8001], 0x0F    ; White text on black background
    
    ; Save parameters on stack (using fixed addresses)
    mov [0x80000], eax
    mov [0x80004], ebx
    
    ; Check for long mode
    mov eax, 0x80000001
    cpuid
    test edx, 0x20000000
    jz error
    
    ; Debug: Write '1' to VGA
    mov byte [0xB8002], '1'
    mov byte [0xB8003], 0x0F
    
    ; Setup simple page tables at fixed addresses
    xor eax, eax
    mov ecx, 16384
    mov edi, 0x10000
    rep stosd
    
    ; PML4 at 0x10000
    mov dword [0x10000], 0x11000 + 3
    
    ; PDPT at 0x11000
    mov dword [0x11000], 0x12000 + 3
    
    ; PD at 0x12000 - map 4GB with 2MB pages
    mov edi, 0x12000
    mov eax, 0x00000083            ; 2MB page, huge, present, write
    mov ecx, 512
.fill_pd:
    mov [edi], eax
    add eax, 0x200000
    add edi, 8
    loop .fill_pd
    
    ; Enable PAE
    mov eax, cr4
    or eax, 0x20
    mov cr4, eax
    
    ; Load CR3
    mov eax, 0x10000
    mov cr3, eax
    
    ; Debug: Write '2' to VGA
    mov byte [0xB8004], '2'
    mov byte [0xB8005], 0x0F
    
    ; Enable long mode
    mov ecx, 0xC0000080
    rdmsr
    or eax, 0x100
    wrmsr
    
    ; Load GDT (absolute address)
    mov dword [0x9F000], 0x00209a0000000000  ; Code segment
    mov dword [0x9F000 + 8], 0x0000920000000000  ; Data segment
    mov word [0x9E000], 23
    mov dword [0x9E000 + 2], 0x9F000
    
    lgdt [0x9E000]
    
    ; Debug: Write '3' to VGA
    mov byte [0xB8006], '3'
    mov byte [0xB8007], 0x0F
    
    ; Enable paging
    mov eax, cr0
    or eax, 0x80000000
    mov cr0, eax
    
    ; Debug: Write '4' to VGA
    mov byte [0xB8008], '4'
    mov byte [0xB8009], 0x0F
    
    jmp 0x08:0x100000
    
error:
    hlt

; Since code is org 0x100000, this is at that address
bits 64

    ; Now in 64-bit mode
    ; Debug: Write '5' to VGA
    mov byte [0xB800A], '5'
    mov byte [0xB800B], 0x0F
    
    mov ax, 0x10
    mov ds, ax
    mov es, ax
    mov fs, ax
    mov gs, ax
    mov ss, ax
    
    ; Load kernel parameters
    mov eax, [0x80000]
    mov ebx, [0x80004]
    
    ; Debug: Write '6' to VGA
    mov byte [0xB800C], '6'
    mov byte [0xB800D], 0x0F

    mov rsi, rax                ; Magic
    mov rdi, rbx                ; Info
    
    ; Call kernel
    call kernel_main
    
    hlt
    jmp $


