#!/bin/bash
# Script to run PuppetOS kernel in QEMU

KERNEL="build/kernel.elf"
QEMU="qemu-system-x86_64"

if [ ! -f "$KERNEL" ]; then
    echo "Error: Kernel not found at $KERNEL"
    echo "Run 'make' to build the kernel first"
    exit 1
fi

echo "Starting PuppetOS kernel in QEMU..."
echo "Press Ctrl+C to exit"
echo ""

# Run QEMU with multiboot2 support
# Use serial output for debugging
$QEMU \
    -m 256 \
    -kernel "$KERNEL" \
    -display gtk \
    -serial stdio \
    -d guest_errors \
    -no-shutdown \
    -no-reboot \
    -cpu qemu64,+x86-64 \
    "$@"
