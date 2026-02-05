# PuppetOS Development Guide

## Quick Start

```bash
# Build the kernel
make

# Run in QEMU (requires GUI or WSL2 with graphics support)
make run

# Debug with GDB
make debug

# Clean build artifacts
make clean
```

## Project Structure Explained

### Boot Stage (`src/boot/boot.s`)
- **Assembly entry point** written in NASM (Intel syntax)
- Loaded by bootloader at physical address ~0x100000
- Sets up 64-bit CPU mode
- Allocates initial stack (16KB)
- Calls `kernel_main()` in C

### Kernel Stage (`src/kernel/main.c`)
- **Kernel entry point** after bootloader transitions to long mode
- Initializes VGA terminal for output
- Parses Multiboot2 information from bootloader
- Initializes subsystems:
  - Physical Memory Manager (PMM) using bitmap
  - Global Descriptor Table (GDT)
  - Interrupt Descriptor Table (IDT)
  - Paging (virtual memory)

### Memory Management
- **PMM** (`src/kernel/memory/pmm.c`): Frame allocation using bitmap
- **GDT/IDT** (`src/kernel/memory/gdt_idt.c`): CPU descriptor tables
- **Types** (`include/types.h`): Freestanding type definitions

### I/O & Output
- **VGA** (`src/kernel/vga.c`): Text mode output driver
- Color support, scrolling, hex printing

## Compilation Pipeline

```
[Source Files]
     ↓
[NASM] → boot.o
[GCC] → *.o (multiple object files)
     ↓
[GNU ld with linker.ld]
     ↓
[kernel.elf] (Executable ELF)
     ↓
[QEMU or Bootloader]
     ↓
[Running Kernel]
```

### Key Files

| File | Purpose |
|------|---------|
| `linker.ld` | Links kernel at high-half (0xFFFFFFFF80000000+) |
| `Makefile` | Orchestrates build process |
| `limine.conf` | Bootloader configuration |
| `include/*.h` | Public header files (freestanding) |
| `src/boot/boot.s` | Assembly entry point (NASM) |
| `src/kernel/main.c` | C kernel entry point |

## Building & Running

### Prerequisites
All tools pre-installed in container:
- `gcc` - C compiler (supports freestanding + x86-64)
- `nasm` - Assembly assembler (Intel syntax)
- `ld` - GNU linker (from binutils)
- `qemu-system-x86_64` - x86_64 emulator
- `gdb` - GNU debugger

### Build Process

1. **Assemble boot code** → `build/obj/boot.o`
2. **Compile C source files** → `build/obj/**/*.o`
3. **Link with linker script** → `build/kernel.elf`

### Important Compiler Flags

```make
-ffreestanding     # No C runtime, no libc
-fno-builtin       # Don't substitute functions
-m64               # 64-bit mode
-mno-red-zone      # ABI compliance (no red zone)
-mno-sse           # No SIMD instructions
```

### Linker Script Details (`linker.ld`)

- **VMA** (Virtual Memory Address): `0xFFFFFFFF80000000` (higher-half)
- **LMA** (Load Memory Address): Physical address where code sits
- **AT()** directive maps physical to virtual addresses
- Bootloader loads kernel at physical ~0x100000
- Kernel code runs at virtual ~0xFFFFFFFF80100000

## Understanding the Memory Layout

The bootloader places the kernel in **low memory** (physical):
```
Physical Memory:
0x000000 ........... 0x100000 ........... 0x400000
[BIOS]              [Kernel loaded here] [end]
```

But the kernel **executes at high virtual addresses**:
```
Virtual Memory:
0x0 ............... 0xFFFFFFFF80100000 (Higher-half kernel)
[Mapped to]         [Code actually runs here]
```

The linker script creates this mapping using `AT()`.

## Testing & Debugging

### Run in QEMU
```bash
make run
```

Kernel output appears on:
- **VGA text buffer** (visible in QEMU window)
- **Serial console** (stdio redirected)

### Debug with GDB
```bash
make debug
```

This:
1. Starts QEMU in debug mode (halted at entry)
2. Connects GDB to QEMU (port :1234)
3. Allows step-through debugging

GDB commands:
```gdb
break kernel_main      # Set breakpoint
continue               # Run to breakpoint
step                   # Step into function
next                   # Step over function
info registers         # Show CPU state
x /16i $rip            # Disassemble at RIP
```

## Extending the Kernel

### To add a new module:

1. **Create source file** in `src/kernel/your_module.c`
2. **Create header** in `include/your_module.h`
3. **Update Makefile** - add to `KERNEL_SRC` variable
4. **Call from main.c** - `#include <your_module.h>`
5. **Rebuild** with `make`

### Example: Adding a timer interrupt handler

```c
// include/timer.h
void timer_init(void);
void timer_handler(void);

// src/kernel/timer.c
#include <timer.h>
#include <memory.h>

void timer_init(void) {
    // Setup timer interrupt
    idt_set_gate(32, timer_handler);
}

void timer_handler(void) {
    // Handle timer tick
}

// src/kernel/main.c
#include <timer.h>
// In kernel_main():
timer_init();
```

## Multiboot2 Protocol

The kernel receives information from bootloader via:
- **RAX register**: Multiboot2 magic number (0x36d76289)
- **RBX register**: Address of multiboot info structure
- **Multiboot info**: Tagged list of boot information

See [`include/multiboot2.h`](../include/multiboot2.h) for tag structures.

## Current Status

✅ **Implemented:**
- Bootloader integration (Limine)
- Assembly entry point
- Stack setup
- C kernel main
- VGA text output
- Physical Memory Manager (bitmap-based)
- Multiboot2 parsing

⚠️ **Stub/Incomplete:**
- GDT/IDT (needs exception handlers)
- Paging (needs page table setup)
- Interrupts (basic structure only)

🔴 **Not Implemented:**
- Keyboard/mouse input
- File system
- Process management
- Multi-core support
- Device drivers

## Development Tips

### Core Dumps
Add debug output with:
```c
vga_print(&terminal, "Debug: ");
vga_print_hex(&terminal, (uint64_t)pointer);
vga_println(&terminal, "");
```

### Register Inspection
In GDB:
```gdb
info registers       # All registers
print $rax           # Specific register
set $rax = 0x1000    # Modify register
```

### Kernel Panic
If kernel hangs, check:
1. Stack overflow (allocate more in boot.s)
2. Page fault (need paging enabled)
3. Triple fault (CPU error - check GDT/IDT)

### Performance
- Kernel is optimized with `-O2`
- No expensive operations in main loop
- VGA output is buffered

## Useful References

- **Multiboot2 Spec**: [GNU Multiboot2](https://www.gnu.org/software/grub/manual/multiboot2/)
- **x86-64 ISA**: [Intel Manuals](https://www.intel.com/content/www/en/en/developer/articles/technical/intel-sdm.html)
- **System V ABI**: [AMD64 ABI](https://refspecs.linuxbase.org/elf/x86_64-abi-0.99.pdf)
- **OS Development**: [OSDev Wiki](https://wiki.osdev.org/)
- **Limine Bootloader**: [GitHub](https://github.com/limine-bootloader/limine)

## Common Issues

### "Error loading uncompressed kernel without PVH ELF Note"
- QEMU's `-kernel` option is picky about Multiboot2 compliance
- Ensure multiboot header is properly aligned and sized
- Current kernel should work with `-kernel` flag in recent QEMU versions

### Build fails with "missing .note.GNU-stack"
- This is just a warning, can be ignored
- It's a legacy stack executable marker
- Doesn't affect functionality in freestanding environment

### Kernel hangs after bootloader
- Check serial output (if available)
- Verify GDT is initialized before any protected mode operations
- Ensure stack is properly allocated in boot.s

## Next Steps for Development

1. **Complete GDT**
   - Set up descriptor for kernel code
   - Implement task switching (TSS)
   
2. **Complete IDT**
   - Exception handlers (divide by zero, page fault, etc.)
   - ISR assembly wrappers
   
3. **Enable Paging**
   - Create page tables
   - Identity map kernel
   - Enable with CR3/CR0
   
4. **Interrupt Handling**
   - Timer interrupt (system clock)
   - Keyboard interrupt
   
5. **I/O & Drivers**
   - Serial port output (more reliable than VGA)
   - IDE/SATA disk drivers
   
6. **Process Management**
   - Process creation (fork-like mechanism)
   - Process scheduling
   - Context switching

Good luck with PuppetOS development!
