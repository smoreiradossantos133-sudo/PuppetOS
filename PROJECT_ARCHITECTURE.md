# PuppetOS - Professional x86_64 Operating System
## Windows 7 Inspired Desktop Environment

```
╔═══════════════════════════════════════════════════════════════╗
║        PuppetOS: Modern Operating System Architecture          ║
║              v0.4.0 - Professional Build                       ║
╚═══════════════════════════════════════════════════════════════╝
```

## Project Structure

### Core Kernel (`kernel/`)
```
kernel/
├── core/
│   ├── kernel.c          - Kernel initialization & logging
│   ├── process.c         - Process/task management
│   └── scheduler.c       - CPU scheduling (round-robin)
├── memory/
│   ├── pmm.c             - Physical Memory Manager
│   ├── vmm.c             - Virtual Memory Manager
│   └── paging.c          - Paging mechanics
├── interrupt/
│   ├── idt.c             - Interrupt Descriptor Table
│   ├── isr.c             - Interrupt Service Routines
│   └── irq.c             - IRQ handling
├── sync/
│   ├── spinlock.c        - Spinlock primitives
│   ├── mutex.c           - Mutex locks
│   └── semaphore.c       - Semaphores
└── ipc/
    ├── pipe.c            - Named pipes
    └── msgqueue.c        - Message queues
```

### Device Drivers (`drivers/`)
```
drivers/
├── input/
│   ├── input.c           - Input management
│   ├── ps2_keyboard.c    - PS/2 Keyboard
│   └── ps2_mouse.c       - PS/2 Mouse
├── display/
│   ├── graphics.c        - Framebuffer rendering
│   ├── vesa.c            - VESA graphics mode
│   └── fonts.c           - Font rendering
├── disk/
│   ├── ata.c             - ATA/SATA controller
│   ├── nvme.c            - NVMe support
│   └── disk_io.c         - Disk I/O
├── network/
│   ├── rtl8139.c         - Realtek NIC driver
│   ├── ethernet.c        - Ethernet layer
│   └── tcp_ip.c          - TCP/IP stack
├── pci.c                 - PCI bus enumeration
└── usb/
    ├── xhci.c            - xHCI host controller
    ├── uhci.c            - UHCI host controller
    └── usb_core.c        - USB core
```

### Filesystem (`fs/`)
```
fs/
├── vfs.c                 - Virtual Filesystem Layer
├── fat32/
│   ├── fat32.c           - FAT32 implementation
│   └── cluster.c         - Cluster management
├── ext2/
│   └── ext2.c            - Ext2 support (future)
└── procfs.c              - Process filesystem
```

### Standard Library (`lib/`)
```
lib/
├── libc/
│   ├── stdio.c           - Standard I/O
│   ├── stdlib.c          - Memory allocation
│   ├── string.c          - String utilities
│   └── math.c            - Math functions
├── gfx/
│   ├── gfx.c             - Graphics library
│   ├── colors.c          - Color utilities
│   └── fonts.c           - Font rendering
├── ui/
│   ├── widgets.c         - UI controls
│   ├── buttons.c         - Button widgets
│   └── dialogs.c         - Dialog boxes
└── util/
    ├── queue.c           - Queue data structure
    ├── hashtable.c       - Hash tables
    └── ringbuffer.c      - Ring buffers
```

### User Interface (`ui/`)
```
ui/
├── desktop/
│   └── desktop.c         - Desktop environment
├── taskbar/
│   └── taskbar.c         - Taskbar/panel
├── wm/
│   └── wm.c              - Window Manager (Aero-style)
└── widgets/
    ├── button.c          - Button widget
    ├── textbox.c         - Text input
    ├── listbox.c         - List control
    └── window_frame.c    - Window decoration
```

### Applications (`apps/`)
```
apps/
├── terminal/
│   └── terminal.c        - Command-line shell
├── explorer/
│   └── explorer.c        - File browser (Windows Explorer-like)
├── notepad/
│   └── notepad.c         - Text editor
├── paint/
│   └── paint.c           - Simple paint app
└── settings/
    └── settings.c        - System settings
```

## Features Implemented ✅

### Bootloader
- [x] Limine boot protocol (modern BIOS/UEFI)
- [x] Higher-half kernel mapping
- [x] Multicore preparation
- [ ] Secure boot support

### Kernel
- [x] 64-bit architecture (x86-64)
- [x] Long mode with paging
- [x] Process management
- [x] Round-robin scheduler
- [x] Basic logging
- [ ] Preemptive scheduling
- [ ] Inter-process communication
- [ ] Virtual memory management

### Memory Management
- [x] Physical Memory Manager (bitmap)
- [x] Page allocation
- [ ] Virtual address space management
- [ ] Memory protection
- [ ] Demand paging

### Device Drivers
- [x] Graphics/Framebuffer
- [x] Input (keyboard/mouse stubs)
- [ ] Full PS/2 protocol
- [ ] ATA/SATA disk
- [ ] Network card
- [ ] USB

### Filesystem
- [x] VFS abstraction
- [ ] FAT32 implementation
- [ ] File operations (read/write)
- [ ] Directory handling
- [ ] Permissions

### User Interface
- [x] Window Manager (Aero-inspired)
- [x] Basic window rendering
- [x] Taskbar/desktop
- [ ] Window decorations
- [ ] Event system
- [ ] Widget library

### Applications
- [x] Terminal/Shell (command-line)
- [ ] File Explorer (browser)
- [ ] Text Editor
- [ ] Paint Application

## Build Instructions

### Requirements
- GCC (x86-64 cross-compiler recommended)
- NASM (assembler)
- GNU Binutils (ld, ar, etc.)
- QEMU (for testing)
- Limine bootloader

### Compile
```bash
cd /workspaces/PuppetOS

# Full build
make clean
make all
make iso-limine

# Run in QEMU
make run-limine
```

### With Limine Bootloader
```bash
# Create ISO with Limine
make iso-limine

# Test
qemu-system-x86_64 -cdrom build/POS-LIMINE.iso -m 512 -display gtk
```

## Architecture Highlights

### Windows 7 Inspired Features
```
Desktop Environment:
┌─────────────────────────────────────────────────────┐
│ File  Edit  View  Tools  Help                    [_][□][×] │
├─────────────────────────────────────────────────────┤
│                                                      │
│      🗂️ Document  📁 Downloads  💾 Programs         │
│                                                      │
│      [PuppetOS Terminal]  [File Explorer]            │
│                                                      │
└─────────────────────────────────────────────────────┘
  [Start] [Terminal] [Explorer] [Settings]     🔔 11:30 AM
```

### Module Dependencies
```
┌──────────────────────────────────────────┐
│         Bootloader (Limine)              │
└──────────────┬───────────────────────────┘
               │
       ┌───────▼──────────┐
       │   Kernel Core    │
       │ (Init, Process)  │
       └───────┬──────────┘
               │
    ┌──────────┼──────────┐
    │          │          │
┌───▼──┐   ┌───▼─┐   ┌───▼────┐
│Memory│   │Intr.│   │Drivers │
│Mgr   │   │   │   │(PCI,Dev)│
└───┬──┘   └─┬───┘   └──┬─────┘
    │        │          │
┌───▼────────▼──────────▼──┐
│    Filesystem (VFS)      │
└───┬──────────────────────┘
    │
┌───▼─────────────────────┐
│   UI/Desktop (WM)       │
└───┬─────────────────────┘
    │
┌───▼─────────────────────┐
│   Applications          │
│(Terminal, Explorer, etc)│
└─────────────────────────┘
```

## Performance Targets

- **Boot Time**: < 2 seconds
- **Memory**: < 10MB for kernel + drivers
- **Multitasking**: 100+ concurrent processes
- **Graphics**: 1024x768@32-bit smooth rendering

## Planned Features (Future)

- [ ] Networking stack (TCP/IP)
- [ ] USB support
- [ ] Audio system
- [ ] Package manager
- [ ] Web browser
- [ ] Email client
- [ ] Networking services (SMB, NFS)
- [ ] 3D graphics support (OpenGL)

## Code Statistics

```
Current: ~50+ source files
Lines of Code: ~15,000+ LOC
Object Modules: 20+
Kernel Size: ~100KB (compressed)
```

## License

PuppetOS is an educational operating system created for learning purposes.
All code is original unless otherwise noted.

## Author

Created: February 2026
Inspired by: Windows 7, Linux, MINIX

---

**Status**: DevelopmentBuild - professional architecture in place,
actively implementing drivers and applications.

Build date: 2026-02-05
Version: 0.4.0-dev
