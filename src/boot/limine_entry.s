# PuppetOS Limine Entry Point
# Limine is a modern bootloader that provides a clean protocol for x86_64 kernels
# This file implements the Limine boot protocol

.code64

# Limine requests section
.section .limine_requests_start, "aw"
.global limine_requests_start
limine_requests_start:

# Framebuffer request
.align 8
framebuffer_request:
    .quad 0x9d5827dcd881dd7b  # ID
    .quad 1                    # Revision
    .quad 0                    # Response (filled by bootloader)

# Memory map request
.align 8
memmap_request:
    .quad 0x67cf3d9d378a806f  # ID
    .quad 0                    # Revision
    .quad 0                    # Response (filled by bootloader)

# RSDP request (for ACPI)
.align 8
rsdp_request:
    .quad 0xc5e77b6b397e7b43  # ID
    .quad 0                    # Revision
    .quad 0                    # Response (filled by bootloader)

.section .limine_requests_end, "aw"
.global limine_requests_end
limine_requests_end:

# Kernel entry point
.section .text
.global _start
.extern kernel_main

_start:
    # We're already in 64-bit long mode with paging enabled
    # Limine provides a complete environment
    
    # Setup stack (Limine provides a stack, but we can set our own)
    mov $0x90000, %rsp
    
    # Save bootloader data structures
    # RAX = magic (0x416d4f4cade48a00 for Limine)
    # RBX = bootloader info pointer
    
    mov %rax, %rdi           # First arg: magic
    mov %rbx, %rsi           # Second arg: info pointer
    
    # Check if we can enable SSE/AVX
    mov $1, %eax
    cpuid
    test $(1 << 25), %edx    # Check SSE support
    jz .no_sse
    
    # Enable SSE
    mov %cr0, %rax
    and $~(1 << 2), %rax     # Clear CR0.EM
    mov %rax, %cr0
    mov %cr4, %rax
    or $(3 << 9), %rax       # Set CR4.OSFXSR and CR4.OSXMMEXCPT
    mov %rax, %cr4
    
.no_sse:
    # Call C kernel main
    # Arguments: RDI = magic, RSI = info
    call kernel_main
    
    # If kernel_main returns, loop forever
    cli
.halt_loop:
    hlt
    jmp .halt_loop

# Ensure we use the kernel stack from Limine
.section .bss
.align 16
stack_space:
    .skip 16384  # 16KB stack
stack_top:
