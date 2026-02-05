#!/bin/bash
# Script to run PuppetOS ISO in QEMU

ISO="build/puppetos.iso"
QEMU="qemu-system-x86_64"

if [ ! -f "$ISO" ]; then
    echo "Error: ISO not found at $ISO"
    echo "Run 'make iso' to create the ISO first"
    exit 1
fi

echo "Starting PuppetOS from ISO in QEMU..."
echo "Press Ctrl+C to exit"
echo ""

# Run QEMU with CD-ROM boot
$QEMU \
    -m 256 \
    -cdrom "$ISO" \
    -display gtk \
    -serial stdio \
    -d guest_errors \
    -no-shutdown \
    -no-reboot \
    "$@"
