# PuppetOS ISO Building Guide

## Overview

PuppetOS now includes **two bootable ISO images**:

1. **puppetos-grub.iso** - Standard GRUB2 bootloader
2. **puppetos-custom.iso** - Custom multi-stage bootloader (without GRUB)

Both ISOs can boot the PuppetOS kernel in QEMU and on physical hardware (with appropriate disk/CD-ROM).

## Build System

### ISO Build Targets

```bash
# Create GRUB-based ISO
make iso

# Create custom bootloader ISO (multi-stage)
make iso-custom

# Create both
make iso iso-custom
```

### Directory Structure

```
build/
├── kernel.elf                  # 64-bit ELF kernel
├── boot/
│   ├── stage1.bin             # MBR bootloader (512 bytes)
│   ├── stage2.bin             # Stage 2 bootloader
│   └── eltorito.bin           # El Torito CD boot image (2048 bytes)
├── puppetos-grub.iso          # ISO with GRUB (5.0 MB)
└── puppetos-custom.iso        # ISO with custom bootloader (1.0 MB)

iso/
├── boot/
│   ├── kernel.elf             # Kernel in ISO
│   ├── grub.cfg               # GRUB configuration
│   └── eltorito.bin           # El Torito image (custom ISO only)
└── EFI/                        # (Optional UEFI support)
```

## Bootloader Architecture

### Stage 1 - MBR Bootloader (512 bytes)
**File**: `src/boot/stage1.s`

- Loaded by BIOS at `0x7C00`
- Real mode (16-bit) operation
- Loads Stage 2 from disk
- Transfers control to Stage 2

**Code Flow**:
```
BIOS → [MBR @ 0x7C00] → Load Stage 2 → Jump to Stage 2
```

### Stage 2 Bootloader
**File**: `src/boot/stage2.s`

- Loaded at `0x0600` by Stage 1
- Real mode (16-bit) operation
- Detects system memory (INT 0x15)
- Enables A20 gate for memory addressing
- Sets up GDT for protected mode
- Transitions to 32-bit protected mode

**Features**:
- Memory detection
- A20 gate control
- GDT loading
- Mode transition (real → protected)

**Code Flow**:
```
[Stage 2 @ 0x0600] → Detect Memory → Enable A20 → 
Load GDT → Enable Protected Mode → Halt
```

### El Torito CD Boot Image (2048 bytes)
**File**: `src/boot/eltorito.s`

- Used for CD-ROM/DVD booting
- Loaded by BIOS during CD boot
- Can emulate floppy or hard disk
- Placed at LBA 0 of ISO image

**Used by**: CD-ROM boot standard (ISO 9660 extension)

### Kernel Entry Point (64-bit ELF)
**File**: `src/boot/boot.s`

- Multiboot2 compliant header
- 64-bit long mode execution
- Calls `kernel_main()` in C
- Allocates 16KB stack

## Creating ISOs

### Method 1: GRUB-Based ISO

**Command**:
```bash
make iso
```

**Process**:
1. Compile kernel
2. Prepare ISO directory structure
3. Copy kernel to `boot/kernel.elf`
4. Create GRUB configuration
5. Use `grub-mkrescue` to generate ISO
6. Add hybrid MBR for USB boot

**Advantages**:
- Standard bootloader
- Widely compatible
- UEFI support (optional)
- Can boot from HDD, USB, CD

**Output**: `build/puppetos-grub.iso` (5.0 MB)

### Method 2: Custom Bootloader ISO

**Command**:
```bash
make iso-custom
```

**Process**:
1. Compile kernel
2. Assemble multi-stage bootloader:
   - Stage 1 (MBR) - 512 bytes
   - Stage 2 - Advanced loader
   - El Torito image - 2048 bytes
3. Create ISO with El Torito boot
4. Add custom MBR for hybrid boot

**Advantages**:
- No external bootloader dependency
- Smaller ISO size (1 MB vs 5 MB)
- Full control over boot process
- Educational value

**Output**: `build/puppetos-custom.iso` (1.0 MB)

## Running ISOs

### QEMU with GRUB ISO
```bash
make run-iso
```

Or manually:
```bash
qemu-system-x86_64 -m 256 -cdrom build/puppetos-grub.iso -display gtk
```

### QEMU with Custom ISO
```bash
make run-iso-custom
```

Or manually:
```bash
qemu-system-x86_64 -m 256 -cdrom build/puppetos-custom.iso -display gtk
```

### Direct Kernel (Fastest for Testing)
```bash
make run
```

This uses QEMU's `-kernel` option (bypasses bootloader):
```bash
qemu-system-x86_64 -kernel build/kernel.elf -m 256 -display gtk
```

## ISO Boot Flow

### GRUB-Based Boot
```
BIOS/UEFI
    ↓
[GRUB Bootloader]
    ├─ Load kernel.elf
    ├─ Setup Multiboot info
    └─ Jump to kernel
         ↓
  [Kernel @ 0xFFFFFFFF80100000]
         ↓
  kernel_main()
```

### Custom Multi-Stage Boot
```
BIOS (CD Boot)
    ↓
[El Torito Loader @ 0x7C00]
    ↓
[Stage 1 - MBR.bin]
    ├─ Load Stage 2
    └─ Jump to Stage 2
         ↓
[Stage 2 Bootloader @ 0x0600]
    ├─ Detect memory
    ├─ Enable A20
    ├─ Load GDT
    ├─ Enter protected mode
    └─ Load kernel (TODO)
         ↓
[Kernel execution]
```

## Tools & Dependencies

### Required Tools
- **NASM** - Assemble bootloader code
- **GCC/Binutils** - Compile kernel
- **xorriso** - Create ISO images
- **grub-mkrescue** - Create GRUB bootdisk
- **QEMU** - Test and run ISOs
- **GDB** - Debug kernel

### Installation
```bash
# All pre-installed in development container
apt-get install nasm build-essential xorriso grub-pc-bin
```

## ISO Specifications

### GRUB ISO Spec
- **Size**: 5.0 MB
- **Format**: ISO 9660 + Rock Ridge + Joliet
- **Boot**: Hybrid MBR (BIOS + USB)
- **Standard**: Multiboot2 partition

### Custom ISO Spec
- **Size**: 1.0 MB
- **Format**: ISO 9660 + El Torito
- **Boot**: Hybrid ISO-MBR (BIOS)
- **Bootloader**: Custom multi-stage
- **Standard**: El Torito without emulation

## Command Reference

```bash
# Build targets
make              # Build kernel only
make iso          # Build GRUB ISO
make iso-custom   # Build custom ISO
make clean        # Delete all artifacts

# Run targets
make run              # Test kernel directly
make run-iso          # Test GRUB ISO
make run-iso-custom   # Test custom ISO
make debug            # GDB debugging

# Help
make help        # Show all targets

# Manual QEMU
qemu-system-x86_64 -m 256 \
    -cdrom build/puppetos-grub.iso \
    -display gtk -serial stdio
```

## License

MIT License - Educational use only

Created: February 5, 2026
