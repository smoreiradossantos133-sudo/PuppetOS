/*
 * Limine Protocol Headers
 * Defines structures and requests for Limine bootloader
 */

#ifndef LIMINE_H
#define LIMINE_H

#include <stdint.h>

#define LIMINE_MAGIC 0x416d4f4cade48a00ULL

/* Framebuffer structure */
struct limine_framebuffer {
    uint64_t address;
    uint64_t width;
    uint64_t height;
    uint64_t pitch;
    uint16_t bpp;
    uint8_t  memory_model;
    uint8_t  red_mask_size;
    uint8_t  red_mask_shift;
    uint8_t  green_mask_size;
    uint8_t  green_mask_shift;
    uint8_t  blue_mask_size;
    uint8_t  blue_mask_shift;
};

struct limine_framebuffer_response {
    uint64_t revision;
    uint64_t framebuffer_count;
    struct limine_framebuffer **framebuffers;
};

/* Memory map entry */
struct limine_memmap_entry {
    uint64_t base;
    uint64_t length;
    uint32_t type;
    uint32_t unused;
    uint64_t acpi_rsdp;
};

struct limine_memmap_response {
    uint64_t revision;
    uint64_t entry_count;
    struct limine_memmap_entry **entries;
};

/* RSDP (ACPI) */
struct limine_rsdp_response {
    uint64_t revision;
    uint64_t *address;
};

/* Memory map types */
#define LIMINE_MEMMAP_USABLE              0
#define LIMINE_MEMMAP_RESERVED            1
#define LIMINE_MEMMAP_ACPI_RECLAIMABLE    2
#define LIMINE_MEMMAP_ACPI_NVS            3
#define LIMINE_MEMMAP_BAD_MEMORY          4
#define LIMINE_MEMMAP_BOOTLOADER_RECLAIMABLE 5
#define LIMINE_MEMMAP_KERNEL_AND_MODULES  6
#define LIMINE_MEMMAP_FRAMEBUFFER         7

/* Bootloader info passed to kernel */
struct limine_bootloader_info {
    uint64_t magic;
    struct limine_framebuffer_response *framebuffer;
    struct limine_memmap_response *memmap;
    struct limine_rsdp_response *rsdp;
};

#endif /* LIMINE_H */
