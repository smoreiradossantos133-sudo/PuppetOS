#!/bin/bash
# Script to create a bootable ISO for PuppetOS without GRUB
# Uses xorriso with proper El Torito configuration

set -e

KERNEL="build/kernel.elf"
ISO_OUTPUT="build/puppetos-bootable.iso"
WORK_DIR="build/iso-work"

if [ ! -f "$KERNEL" ]; then
    echo "❌ Error: Kernel not found at $KERNEL"
    echo "   Run 'make all' first"
    exit 1
fi

echo "📀 Creating bootable ISO..."

# Clean work directory
rm -rf "$WORK_DIR"
mkdir -p "$WORK_DIR/boot"

# Copy kernel
cp "$KERNEL" "$WORK_DIR/boot/kernel.elf"

# Create GRUB configuration
mkdir -p "$WORK_DIR/boot/grub"
cat > "$WORK_DIR/boot/grub/grub.cfg" << 'EOF'
set gfxmode=1024x768
insmod gfx_term
insmod font
if loadfont "$prefix/fonts/unicode.pf2" ; then
  set gfxmode=1024x768
  terminal_output gfxterm
fi

set timeout=5
set default=0

menuentry "PuppetOS 64-bit" {
  multiboot2 /boot/kernel.elf
  boot
}

menuentry "QEMU Reboot" {
  reboot
}
EOF

# Create ISO using grub-mkrescue
echo "  Building ISO with grub-mkrescue..."
grub-mkrescue -o "$ISO_OUTPUT" "$WORK_DIR" 2>&1 | while read line; do
    if [[ "$line" =~ (ERROR|xorriso|bytes|saved) ]]; then
        echo "  $line"
    fi
done

if [ -f "$ISO_OUTPUT" ]; then
    SIZE=$(ls -lh "$ISO_OUTPUT" | awk '{print $5}')
    echo "✅ ISO created: $ISO_OUTPUT ($SIZE)"
    
    # Test with QEMU
    echo ""
    echo "🚀 Testing ISO in QEMU..."
    echo "   Use Ctrl+C to stop"
    timeout 60 qemu-system-x86_64 \
        -cdrom "$ISO_OUTPUT" \
        -m 256 \
        -display none \
        -serial stdio \
        -nographic \
        2>&1 | head -100 || true
else
    echo "❌ Failed to create ISO"
    exit 1
fi
