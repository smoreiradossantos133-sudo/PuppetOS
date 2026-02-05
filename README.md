# PuppetOS - A 64-bit Operating System from Scratch

PuppetOS is a monolithic, 64-bit operating system kernel built from scratch with a focus on learning and understanding low-level system architecture.

## Architecture Overview

PuppetOS is designed as a **monolithic kernel** targeting the **x86_64 (AMD64)** architecture with **UEFI/Multiboot2** bootloader support. The kernel manages:

- **Physical Memory Management (PMM)** using a bitmap allocator
- **Virtual Memory & Paging**
- **Global Descriptor Table (GDT)** for memory protection
- **Interrupt Descriptor Table (IDT)** for exception handling
- **VGA Text Mode** or **Framebuffer Output** for console I/O

## Project Structure

```
PuppetOS/
├── src/
│   ├── boot/
│   │   └── boot.s              # Assembly entry point (NASM)
│   ├── kernel/
│   │   ├── main.c              # Kernel main function
│   │   ├── vga.c               # VGA/Framebuffer output
│   │   └── memory/
│   │       ├── pmm.c           # Physical Memory Manager
│   │       └── gdt_idt.c       # GDT/IDT initialization
│   └── limine/                 # Limine bootloader binaries
├── include/
│   ├── types.h                 # Basic type definitions (freestanding)
│   ├── multiboot2.h            # Multiboot2 structures
│   ├── memory.h                # Memory management declarations
│   └── vga.h                   # VGA output functions
├── build/                      # Build artifacts
├── Makefile                    # Build system
├── linker.ld                   # Linker script
└── limine.conf                 # Bootloader configuration
```

## Build System & Compilation

### Prerequisites
- **GCC** (cross-compiler for x86_64-elf)
- **NASM** (Assing Assembler)
- **GNU Binutils** (ld linker)
- **QEMU** (emulator for testing)
- **GDB** (debugger)
- **xorriso** & **grub-pc-bin** (for ISO creation)

All tools are pre-installed in the development container.

### Building the Kernel

```bash
# Build kernel ELF binary
make

# Run kernel in QEMU
make run

# Debug with GDB
make debug

# Create bootable ISO image
make iso

# Clean build artifacts
make clean

# Show help
make help
```

## Compiler & Assembler Flags

### C Compiler Flags
```
-ffreestanding        # No standard library
-fno-stack-protector  # Disable stack protector
-fno-builtin          # Don't use builtin functions
-m64                  # 64-bit mode
-mno-red-zone         # No red zone (System V ABI)
-mno-sse              # Disable SSE/SSE2
```

### Assembly Format
- **Syntax**: NASM (Intel-style)
- **Format**: `elf64` (64-bit ELF object files)

### Linker Script
The [linker.ld](linker.ld) script:
- Places kernel at `0xFFFFFFFF80100000` (higher-half kernel)
- Maps sections in memory with proper alignment
- Provides physical address mapping for bootloader using `AT()`

## Kernel Entry Flow

1. **Bootloader** (Limine/GRUB) loads kernel ELF
2. **Assembly Code** ([boot.s](src/boot/boot.s)) sets up:
   - Stack pointer
   - CPU in 64-bit mode
   - Passes control to C code
3. **Kernel Main** ([main.c](src/kernel/main.c)) initializes:
   - VGA terminal for output
   - Parses Multiboot2 information
   - Initializes Physical Memory Manager
   - Sets up GDT, IDT, and paging
   - Enters main kernel loop

## Key Features

### Physical Memory Manager (PMM)
- Bitmap-based frame allocator
- Tracks free/used pages
- Frame size: 4KB (PAGE_SIZE)
- Maximum addressable: 32GB

### VGA Terminal
- 80x25 text mode
- Color support (16 colors)
- Text scrolling
- Hex number printing for debugging

### Memory Management
- **GDT**: Global Descriptor Table setup (stub)
- **IDT**: Interrupt descriptor table (stub)
- **Paging**: Virtual memory initialization (stub)

## Freestanding Environment

PuppetOS is a **freestanding implementation**:
- ✅ No `libc` (standard C library)
- ✅ No dynamic linking
- ✅ Custom type definitions ([types.h](include/types.h))
- ✅ No system calls (yet)
- ✅ Minimal runtime

## Testing & Debugging

### QEMU Emulation
```bash
make run           # Graphical mode
```

### GDB Debugging
```bash
make debug         # Connects GDB to QEMU at port 1234
```

In GDB, useful commands:
```gdb
target remote :1234
break kernel_main
continue
step / next        # Single step / Step over
info registers     # Show CPU registers
x /16i $rip        # Disassemble at RIP
```

### Observing Output
The kernel prints startup messages to the VGA text buffer and serial console.

## Bootloader: Limine

PuppetOS uses **Limine bootloader** (v7.x) for boot management:
- Multiboot2 compliant
- UEFI support
- Framebuffer support
- Configuration via [limine.conf](limine.conf)

## Memory Layout

```
0x000000  +─────────────────────────────┐
          │     Real Mode / BIOS        │
          │     (first 1MB)             │
          │                             │
0x100000  +─────────────────────────────┐  ← Kernel starts here (physical)
          │     Kernel (bootloader      │
          │     loads here)             │
          │                             │
          │  [code, data, stack, etc]   │
          │                             │
0xFFFFFFFF80100000 (virtual address)
          ↓ (higher-half kernel mapping)
```

## Future Enhancements

- [ ] Full GDT/LDT implementation
- [ ] Exception handling and interrupts
- [ ] Virtual memory & paging complete implementation
- [ ] Keyboard & mouse input
- [ ] Filesystem support
- [ ] Process management
- [ ] Multiprocessing (SMP)
- [ ] Framebuffer graphics mode

## References

- [Multiboot2 Specification](https://www.gnu.org/software/grub/manual/multiboot2/)
- [OSDev.org](https://wiki.osdev.org/)
- [Intel x86-64 Architecture](https://www.intel.com/content/dam/www/public/us/en/documents/manuals/64-ia-32-architectures-software-developer-manual-325462.pdf)
- [System V AMD64 ABI](https://refspecs.linuxbase.org/elf/x86_64-abi-0.99.pdf)
- [Limine Bootloader](https://github.com/limine-bootloader/limine)

## Author

Built from scratch following OS architecture principles.

## License

MIT License - Educational use only
