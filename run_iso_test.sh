#!/bin/bash
# Test PuppetOS ISO with QEMU

ISO_FILE="build/PuppetOSVersionLikeWindows7.iso"

if [ ! -f "$ISO_FILE" ]; then
    echo "❌ ISO not found: $ISO_FILE"
    exit 1
fi

echo "🚀 Booting PuppetOS with QEMU (Limine)..."
echo "   To quit QEMU: Ctrl+Alt+Q"
echo ""

qemu-system-x86_64 \
    -cdrom "$ISO_FILE" \
    -m 256M \
    -enable-kvm \
    -cpu host \
    -serial stdio \
    -monitor telnet:127.0.0.1:55555,server,nowait

