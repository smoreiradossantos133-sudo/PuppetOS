; PuppetOS - El Torito Boot Image
; This is a 2KB boot image used for CD-ROM booting
; It emulates a 3.5" floppy disk

bits 16
org 0x0000

; El Torito boot image header
; This image will be loaded at 0x07C0:0x0000 (0x7C00)

start:
    jmp boot_entry
    nop
    
boot_entry:
    cli
    cld
    
    ; Setup segments (El Torito loads us at 0x7C00)
    xor ax, ax
    mov ds, ax
    mov es, ax
    mov ss, ax
    mov sp, 0x7C00
    
    sti
    
    ; Print boot message
    mov si, msg_eltorito
    call print_string
    
    ; For El Torito CD boot, kernel should be loaded from the ISO
    ; In qemu, we can use -cdrom option
    
    mov si, msg_ready
    call print_string
    
    ; Jump to kernel if loaded, otherwise halt
    ; Check if kernel signature exists at 0x80000
    mov ax, 0x8000
    mov ds, ax
    cmp word [0], 0x7F45    ; ELF magic
    jne no_kernel
    
    ; Kernel found - jump to it
    ; Setup for kernel entry
    jmp 0x8000:0x0000
    
no_kernel:
    mov si, msg_no_kernel
    call print_string
    
    jmp halt

print_string:
    lodsb
    or al, al
    jz .done
    mov bx, 0x0007
    mov ah, 0x0E
    int 0x10
    jmp print_string
.done:
    ret

halt:
    hlt
    jmp halt

; Data
msg_eltorito:    db "PuppetOS El Torito Boot", 0x0D, 0x0A, 0
msg_ready:       db "Ready to boot kernel...", 0x0D, 0x0A, 0
msg_no_kernel:   db "Error: Kernel not found!", 0x0D, 0x0A, 0

; Pad to 2048 bytes (El Torito requirement)
times (2048 - ($ - $$)) db 0
