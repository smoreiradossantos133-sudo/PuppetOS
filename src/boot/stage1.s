; PuppetOS - Stage 1 Bootloader (MBR)
; This code runs in 16-bit real mode at 0x7C00
; Purpose: Load Stage 2 bootloader from disk and transfer control

bits 16
org 0x7C00

; ===== MBR BOOTLOADER STAGE 1 =====

start:
    cli                         ; Disable interrupts
    cld                         ; Clear direction flag
    
    ; Setup segments
    xor ax, ax
    mov ds, ax
    mov es, ax
    mov ss, ax
    mov sp, 0x7C00              ; Stack below bootloader
    
    sti                         ; Enable interrupts
    
    ; Save boot drive in DL
    mov [boot_drive], dl
    
    ; Print loading message
    mov si, msg_loading
    call print_string
    
    ; Load Stage 2 from disk (LBA 1)
    ; For now, load from disk sector 1 to 0x0600
    mov ax, 0x0201              ; Read 1 sector
    mov bx, 0x0600              ; Load address
    mov cx, 0x0002              ; Cylinder 0, Sector 2 (1-indexed)
    mov dx, 0x0000              ; Head 0, Drive 0
    int 0x13                    ; Disk BIOS interrupt
    
    jc disk_error
    
    ; Jump to Stage 2
    mov si, msg_loaded
    call print_string
    
    jmp 0x0000:0x0600           ; Jump to stage 2
    
disk_error:
    mov si, msg_error
    call print_string
    jmp halt

print_string:
    ; Print string at DS:SI
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
boot_drive:     db 0
msg_loading:    db "PuppetOS Bootloader Stage 1", 0x0D, 0x0A, 0
msg_loaded:     db "Loading Stage 2...", 0x0D, 0x0A, 0
msg_error:      db "Disk Error!", 0x0D, 0x0A, 0

; Pad to 510 bytes and add boot signature
times (510 - ($ - $$)) db 0
dw 0xAA55                       ; Boot sector signature
