# Makefile for PuppetOS
# Build system for compiling 64-bit kernel and creating bootable ISO
# Supports multiple bootloaders: GRUB and custom multi-stage

.PHONY: all clean iso iso-custom iso-limine run run-iso run-iso-custom run-limine debug help

# Tools
CC = gcc
LD = ld
NASM = nasm
QEMU = qemu-system-x86_64
GDB = gdb
DD = dd
XORRISO = xorriso

# Compiler flags
CFLAGS = -O2 -Wall -Wextra -ffreestanding -fno-stack-protector \
         -fno-builtin -m64 -march=x86-64 -mno-red-zone -mno-mmx \
         -mno-sse -mno-sse2 -I./include

# Assembler flags for different modes
ASFLAGS_16 = -f bin     # 16-bit real mode (flat binary)
ASFLAGS_32 = -f elf32   # 32-bit protected mode (ELF)
ASFLAGS_64 = -f elf64   # 64-bit long mode (ELF)

# Linker flags
LDFLAGS = -T linker.ld -nostdlib -z max-page-size=0x1000

# Directory structure
SRC_DIR = src
BUILD_DIR = build
ISO_DIR = iso
OBJ_DIR = $(BUILD_DIR)/obj
BOOT_DIR = $(BUILD_DIR)/boot

# Source files
BOOT_SRC = $(SRC_DIR)/boot/boot.s
BOOT_LIMINE_SRC = $(SRC_DIR)/boot/boot_limine.s
STAGE1_SRC = $(SRC_DIR)/boot/stage1.s
STAGE2_SRC = $(SRC_DIR)/boot/stage2.s
ELTORITO_SRC = $(SRC_DIR)/boot/eltorito.s

KERNEL_SRC = $(SRC_DIR)/kernel/main.c \
             $(SRC_DIR)/kernel/vga.c \
             $(SRC_DIR)/kernel/memory/pmm.c \
             $(SRC_DIR)/kernel/memory/gdt_idt.c

KERNEL_LIMINE_SRC = $(SRC_DIR)/kernel/main_limine.c \
					$(SRC_DIR)/kernel/vga.c \
					$(SRC_DIR)/kernel/memory/pmm.c \
					$(SRC_DIR)/kernel/memory/gdt_idt.c \
					$(SRC_DIR)/kernel/core/kernel.c \
					$(SRC_DIR)/kernel/core/process.c \
					$(SRC_DIR)/drivers/display/graphics.c \
					$(SRC_DIR)/drivers/input/input.c \
					$(SRC_DIR)/ui/wm/wm.c \
							$(SRC_DIR)/apps/terminal/terminal.c \
							$(SRC_DIR)/libc/malloc.c \
							$(SRC_DIR)/libc/string.c

# Object files
BOOT_OBJ = $(OBJ_DIR)/boot.o
BOOT_LIMINE_OBJ = $(OBJ_DIR)/boot_limine.o
KERNEL_OBJ = $(patsubst %.c,$(OBJ_DIR)/%.o,$(patsubst $(SRC_DIR)/%,%,$(KERNEL_SRC)))
KERNEL_LIMINE_OBJ = $(patsubst %.c,$(OBJ_DIR)/%.o,$(patsubst $(SRC_DIR)/%,%,$(KERNEL_LIMINE_SRC)))

# Binary outputs
KERNEL_ELF = $(BUILD_DIR)/kernel.elf
KERNEL_LIMINE_ELF = $(BUILD_DIR)/kernel-limine.elf
STAGE1_BIN = $(BOOT_DIR)/stage1.bin
STAGE2_BIN = $(BOOT_DIR)/stage2.bin
ELTORITO_BIN = $(BOOT_DIR)/eltorito.bin

# ISO outputs
ISO_GRUB = $(BUILD_DIR)/POS-GRUB.iso
ISO_CUSTOM = $(BUILD_DIR)/puppetos-custom.iso
ISO_LIMINE = $(BUILD_DIR)/PuppetOSVersionLikeWindows7.iso

# Default target
all: $(KERNEL_ELF)
	@echo ""
	@echo "✅ Kernel built successfully!"
	@echo "   Use 'make help' for build options"

# Create directories
$(OBJ_DIR) $(BOOT_DIR) $(BUILD_DIR) $(ISO_DIR):
	@mkdir -p $@ $(OBJ_DIR)/kernel $(OBJ_DIR)/kernel/core $(OBJ_DIR)/kernel/memory $(OBJ_DIR)/drivers $(OBJ_DIR)/drivers/input $(OBJ_DIR)/drivers/display $(OBJ_DIR)/ui $(OBJ_DIR)/ui/wm $(OBJ_DIR)/apps $(OBJ_DIR)/apps/terminal

# ===== KERNEL BUILD =====

# Compile kernel boot.s
$(BOOT_OBJ): $(BOOT_SRC) | $(OBJ_DIR)
	@echo "  [AS] Kernel boot (ELF64)"
	@$(NASM) $(ASFLAGS_64) $< -o $@

# Compile C source files
$(OBJ_DIR)/%.o: $(SRC_DIR)/%.c | $(OBJ_DIR)
	@mkdir -p $(dir $@)
	@echo "  [CC] $<"
	@$(CC) $(CFLAGS) -c $< -o $@

# Compile C source files located in project root or other dirs
$(OBJ_DIR)/%.o: %.c | $(OBJ_DIR)
	@mkdir -p $(dir $@)
	@echo "  [CC] $<"
	@$(CC) $(CFLAGS) -c $< -o $@

# Link kernel ELF
$(KERNEL_ELF): $(BOOT_OBJ) $(KERNEL_OBJ) | $(BUILD_DIR)
	@echo "  [LD] Linking kernel..."
	@$(LD) $(LDFLAGS) $(BOOT_OBJ) $(KERNEL_OBJ) -o $@
	@echo "✅ Kernel: $(KERNEL_ELF)"

# ===== LIMINE BOOTLOADER BUILD =====

# Compile Limine boot.s
$(BOOT_LIMINE_OBJ): $(BOOT_LIMINE_SRC) | $(OBJ_DIR)
	@echo "  [AS] Kernel boot (Limine)"
	@$(NASM) $(ASFLAGS_64) $< -o $@

# Link kernel ELF with Limine
$(KERNEL_LIMINE_ELF): $(BOOT_LIMINE_OBJ) $(KERNEL_LIMINE_OBJ) | $(BUILD_DIR)
	@echo "  [LD] Linking kernel (Limine)..."
	@$(LD) $(LDFLAGS) $(BOOT_LIMINE_OBJ) $(KERNEL_LIMINE_OBJ) -o $@
	@echo "✅ Kernel (Limine): $(KERNEL_LIMINE_ELF)"

# ===== BOOTLOADER BINARIES =====

# Stage 1 MBR (512 bytes)
$(STAGE1_BIN): $(STAGE1_SRC) | $(BOOT_DIR)
	@echo "  [AS] Stage 1 MBR (512 bytes)"
	@$(NASM) $(ASFLAGS_16) $< -o $@
	@size=$$(stat -c%s $@ 2>/dev/null || stat -f%z $@ 2>/dev/null); \
	if [ $$size -gt 512 ]; then echo "❌ Stage 1 too large: $$size > 512"; exit 1; fi

# Stage 2 bootloader
$(STAGE2_BIN): $(STAGE2_SRC) | $(BOOT_DIR)
	@echo "  [AS] Stage 2 Bootloader"
	@$(NASM) $(ASFLAGS_16) $< -o $@

# El Torito boot image (2048 bytes)
$(ELTORITO_BIN): $(ELTORITO_SRC) | $(BOOT_DIR)
	@echo "  [AS] El Torito Boot Image (2048 bytes)"
	@$(NASM) $(ASFLAGS_16) $< -o $@
	@size=$$(stat -c%s $@ 2>/dev/null || stat -f%z $@ 2>/dev/null); \
	if [ $$size -ne 2048 ]; then echo "❌ El Torito wrong size: $$size != 2048"; exit 1; fi

# ===== ISO IMAGES =====

# ISO with GRUB bootloader
iso: $(KERNEL_ELF)
	@echo ""
	@echo "🔨 Creating bootable ISO with GRUB..."
	@mkdir -p $(ISO_DIR)/boot/grub
	@cp $(KERNEL_ELF) $(ISO_DIR)/boot/kernel.elf
	@echo 'menuentry "PuppetOS 64-bit" { multiboot2 /boot/kernel.elf; boot }' \
		> $(ISO_DIR)/boot/grub/grub.cfg
	@echo "📀 Building ISO image..."
	@grub-mkrescue -o $(ISO_GRUB) $(ISO_DIR) 2>&1 | grep -v "^This" || true
	@if [ -f $(ISO_GRUB) ]; then \
		echo "✅ GRUB ISO: $(ISO_GRUB)"; \
		ls -lh $(ISO_GRUB); fi
	@echo ""

# ISO with Limine bootloader (MODERN & RECOMMENDED!)
iso-limine: $(KERNEL_LIMINE_ELF)
	@echo ""
	@echo "🔨 Creating bootable ISO with Limine..."
	@mkdir -p $(ISO_DIR)/boot
	@cp $(KERNEL_LIMINE_ELF) $(ISO_DIR)/boot/kernel.elf
	@cp limine.conf $(ISO_DIR)/limine.conf
	@cp src/limine/limine-bios.sys $(ISO_DIR)/
	@cp src/limine/limine-bios-cd.bin $(ISO_DIR)/limine-bios-cd.bin
	@cp src/limine/BOOTX64.EFI $(ISO_DIR)/BOOTX64.EFI
	@echo "📀 Building ISO image..."
	@cd $(ISO_DIR) && $(XORRISO) -as mkisofs \
		-R -J -b limine-bios-cd.bin \
		-no-emul-boot -boot-load-size 4 -boot-info-table \
		-efi-boot BOOTX64.EFI \
		-efi-boot-partition false \
		-append_partition 2 0xef EFI.img \
		-o ../$(ISO_LIMINE) . 2>&1 | tail -3 || true
	@if [ -f $(ISO_LIMINE) ]; then \
		echo "✅ Limine ISO: $(ISO_LIMINE)"; \
		ls -lh $(ISO_LIMINE); fi
	@echo ""

# ISO with custom multi-stage bootloader
iso-custom: $(KERNEL_ELF) $(STAGE1_BIN) $(STAGE2_BIN) $(ELTORITO_BIN)
	@echo ""
	@echo "🔨 Creating custom bootable ISO (multi-stage bootloader)..."
	@mkdir -p $(ISO_DIR)/boot
	@cp $(KERNEL_ELF) $(ISO_DIR)/boot/kernel.elf
	@cp $(ELTORITO_BIN) $(ISO_DIR)/eltorito.bin
	@echo "📀 Building El Torito ISO..."
	@cd $(ISO_DIR) && \
	$(XORRISO) -as mkisofs -R -J \
		-isohybrid-mbr ../$(STAGE1_BIN) \
		-c boot.cat \
		-b eltorito.bin \
		-no-emul-boot \
		-boot-load-size 4 \
		-boot-info-table \
		-o ../$(ISO_CUSTOM) \
		. 2>&1 | tail -5
	@if [ -f $(ISO_CUSTOM) ]; then \
		echo "✅ Custom ISO: $(ISO_CUSTOM)"; \
		ls -lh $(ISO_CUSTOM); fi
	@echo ""

# ===== QEMU RUN TARGETS =====

# Run kernel directly (fastest)
run: $(KERNEL_ELF)
	@echo ""
	@echo "🚀 Running PuppetOS in QEMU (direct kernel)..."
	@echo "   Press Ctrl+C to exit"
	@echo ""
	@$(QEMU) -kernel $(KERNEL_ELF) \
	         -m 256M \
	         -display gtk \
	         -serial stdio \
	         -d guest_errors \
	         -no-shutdown -no-reboot

# Run GRUB ISO
run-iso: iso
	@echo ""
	@echo "🚀 Running PuppetOS in QEMU (GRUB bootloader)..."
	@echo "   Press Ctrl+C to exit"
	@echo ""
	@$(QEMU) -m 256M \
	         -cdrom $(ISO_GRUB) \
	         -display gtk \
	         -serial stdio \
	         -d guest_errors \
	         -no-shutdown -no-reboot

# Run custom ISO
run-iso-custom: iso-custom
	@echo ""
	@echo "🚀 Running PuppetOS in QEMU (custom bootloader)..."
	@echo "   Press Ctrl+C to exit"
	@echo ""
	@$(QEMU) -m 256M \
	         -cdrom $(ISO_CUSTOM) \
	         -display gtk \
	         -serial stdio \
	         -d guest_errors \
	         -no-shutdown -no-reboot

# Run Limine ISO (RECOMMENDED!)
run-limine: iso-limine
	@echo ""
	@echo "🚀 Running PuppetOS in QEMU (Limine bootloader)..."
	@echo "   ⭐ Modern bootloader - should boot successfully!"
	@echo "   Press Ctrl+C to exit"
	@echo ""
	@$(QEMU) -m 256M \
	         -cdrom $(ISO_LIMINE) \
	         -display gtk \
	         -serial stdio \
	         -d guest_errors \
	         -no-shutdown -no-reboot

# ===== DEBUG =====

# Debug with GDB
debug: $(KERNEL_ELF)
	@echo ""
	@echo "🐛 Debug mode: QEMU waiting for GDB connection..."
	@$(QEMU) -kernel $(KERNEL_ELF) \
	         -m 256M \
	         -display gtk \
	         -S -gdb tcp::1234 \
	         -d guest_errors &
	@sleep 1
	@echo "GDB connecting to QEMU..."
	@$(GDB) $(KERNEL_ELF) \
	        -ex "target remote :1234" \
	        -ex "break kernel_main" \
	        -ex "continue"

# ===== MAINTENANCE =====

# Clean all artifacts
clean:
	@echo "🧹 Cleaning build artifacts..."
	@rm -rf $(BUILD_DIR) $(ISO_DIR)
	@echo "✅ Clean complete"

# Show help
help:
	@echo ""
	@echo "╔══════════════════════════════════════════════════════════╗"
	@echo "║        PuppetOS: Modern Bootloader Build System           ║"
	@echo "╚══════════════════════════════════════════════════════════╝"
	@echo ""
	@echo "📦 BUILD TARGETS:"
	@echo "   make              - Build kernel (default)"
	@echo "   make iso          - Create GRUB bootdisk ISO"
	@echo "   make iso-custom   - Create custom bootloader ISO"
	@echo "   make iso-limine   - Create Limine bootdisk ISO ⭐"
	@echo ""
	@echo "▶️  RUN TARGETS:"
	@echo "   make run               - Run kernel directly in QEMU"
	@echo "   make run-iso           - Boot GRUB ISO in QEMU"
	@echo "   make run-iso-custom    - Boot custom ISO in QEMU"
	@echo "   make run-limine        - Boot Limine ISO in QEMU ⭐"
	@echo "   make debug             - Debug kernel with GDB"
	@echo ""
	@echo "🧹 MAINTENANCE:"
	@echo "   make clean   - Remove all build artifacts"
	@echo "   make help    - Show this message"
	@echo ""
	@echo "🚀 RECOMMENDED WORKFLOW (Limine):"
	@echo "   $$ make run-limine"
	@echo ""
	@echo "💡 BOOTLOADERS:"
	@echo "   • Limine ⭐ - Modern, UEFI+BIOS, recommended"
	@echo "   • GRUB - Classic, widely compatible"
	@echo "   • Custom - Educational (Stage1/2 + El Torito)"
	@echo ""

.DEFAULT_GOAL := help

