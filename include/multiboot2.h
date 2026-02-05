#ifndef __MULTIBOOT2_H__
#define __MULTIBOOT2_H__

#include <types.h>

/* Multiboot2 header structure */
#define MULTIBOOT2_HEADER_MAGIC     0xe85250d6
#define MULTIBOOT2_BOOTLOADER_MAGIC 0x36d76289

/* Multiboot2 tag types */
#define MULTIBOOT_TAG_TYPE_END           0
#define MULTIBOOT_TAG_TYPE_CMDLINE       1
#define MULTIBOOT_TAG_TYPE_BOOT_LOADER   2
#define MULTIBOOT_TAG_TYPE_MODULE        3
#define MULTIBOOT_TAG_TYPE_BASIC_MEMINFO 4
#define MULTIBOOT_TAG_TYPE_BOOTDEV       5
#define MULTIBOOT_TAG_TYPE_MMAP          6
#define MULTIBOOT_TAG_TYPE_VBE           7
#define MULTIBOOT_TAG_TYPE_FRAMEBUFFER   8

typedef struct {
    uint32_t type;
    uint32_t size;
} multiboot_tag_t;

typedef struct {
    uint32_t mem_lower;  /* Amount of lower memory in KB */
    uint32_t mem_upper;  /* Amount of upper memory in KB */
} multiboot_basic_meminfo_t;

typedef struct {
    uint32_t type;
    uint32_t size;
    uint32_t entry_size;
    uint32_t entry_version;
} multiboot_mmap_tag_t;

typedef struct {
    uint64_t addr;
    uint64_t len;
    uint32_t type;
    uint32_t zero; /* used to be reserved */
} multiboot_mmap_entry_t;

typedef struct {
    uint32_t type;
    uint32_t size;
    uint64_t framebuffer_addr;
    uint32_t framebuffer_pitch;
    uint32_t framebuffer_width;
    uint32_t framebuffer_height;
    uint8_t framebuffer_bpp;
} multiboot_framebuffer_tag_t;

#endif /* __MULTIBOOT2_H__ */
