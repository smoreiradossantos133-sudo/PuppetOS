; PuppetOS - Stage 2 Bootloader
; Loaded at 0x0600 by Stage 1
; Purpose: Load kernel and prepare environment for 64-bit mode

bits 16
org 0x0600

jmp start

; ===== DATA STRUCTURES =====

gdt:
    ; NULL descriptor
    dq 0
    
    ; Code descriptor (64-bit)
    dw 0xFFFF               ; Limit
    dw 0                    ; Base 0-15
    db 0                    ; Base 16-23
    db 0x9A                 ; Present, Ring 0, Code, Readable
    db 0xAF                 ; Granularity, 64-bit
    db 0                    ; Base 24-31
    
    ; Data descriptor (64-bit)
    dw 0xFFFF
    dw 0
    db 0
    db 0x92                 ; Present, Ring 0, Data, Writable
    db 0xAF
    db 0

gdt_pointer:
    dw ($ - gdt) - 1        ; GDT size - 1
    dq gdt                  ; GDT address

; ===== STAGE 2 START =====

start:
    cli
    cld
    
    ; Setup segments
    mov ax, cs
    mov ds, ax
    mov es, ax
    
    ; Print stage 2 message
    mov si, msg_stage2
    call print_string
    
    ; Detect memory size
    call detect_memory
    
    ; Load kernel from disk
    ; For ISO: load from specific location
    mov si, msg_loading_kernel
    call print_string
    
    ; Enable A20 line (gate 20 memory line)
    call enable_a20
    
    ; Load GDT for protected mode
    lgdt [gdt_pointer]
    
    ; Enable protected mode
    mov eax, cr0
    or eax, 1
    mov cr0, eax
    
    ; Jump to protected mode
    jmp 0x08:protected_mode
    
detect_memory:
    ; Use BIOS INT 15h to get memory
    ; Returns EAX = extended memory in KB (> 1MB)
    mov ax, 0xE801
    int 0x15
    
    ; For now, just return
    ret

enable_a20:
    ; Enable A20 line using keyboard controller
    call a20_wait_input
    mov al, 0xAD
    out 0x64, al            ; Disable keyboard
    
    call a20_wait_input
    mov al, 0xD0
    out 0x64, al            ; Read controller output port
    
    call a20_wait_output
    in al, 0x60
    mov ah, al              ; Save output port value
    
    call a20_wait_input
    mov al, 0xD1
    out 0x64, al            ; Write controller output port
    
    call a20_wait_input
    mov al, ah
    or al, 0x02             ; Set A20 bit
    out 0x60, al
    
    call a20_wait_input
    mov al, 0xAE
    out 0x64, al            ; Enable keyboard
    ret

a20_wait_input:
    in al, 0x64
    test al, 2
    jnz a20_wait_input
    ret

a20_wait_output:
    in al, 0x64
    test al, 1
    jz a20_wait_output
    ret

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

; Data
msg_stage2:         db "Entering Stage 2 Bootloader", 0x0D, 0x0A, 0
msg_loading_kernel: db "Loading kernel...", 0x0D, 0x0A, 0

; ===== PROTECTED MODE CODE =====

bits 32
protected_mode:
    mov ax, 0x10
    mov ds, ax
    mov es, ax
    mov fs, ax
    mov gs, ax
    mov ss, ax
    
    mov esp, 0x90000       ; Stack at 576KB
    
    ; For now, just halt
    ; In full implementation, would:
    ; - Load kernel from disk
    ; - Setup page tables
    ; - Enter long mode (64-bit)
    ; - Jump to kernel
    
    hlt
    jmp $
