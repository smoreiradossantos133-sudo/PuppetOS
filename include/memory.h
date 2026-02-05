#ifndef __MEMORY_H__
#define __MEMORY_H__

#include <types.h>

/* Physical Memory Manager (PMM) Configuration */
#define PAGE_SIZE               4096
#define PMM_BITMAP_SIZE         (1024 * 1024)  /* 1MB bitmap = 8M pages = 32GB memory */
#define PMM_MAX_FRAMES          (PMM_BITMAP_SIZE * 8)

typedef struct {
    uint8_t *bitmap;
    uint32_t num_frames;
    uint32_t used_frames;
} pmm_t;

/* PMM Functions */
void pmm_init(pmm_t *pmm, uint32_t total_frames);
uint32_t pmm_alloc_frame(pmm_t *pmm);
void pmm_free_frame(pmm_t *pmm, uint32_t frame);
void pmm_mark_frame_used(pmm_t *pmm, uint32_t frame);
void pmm_mark_frame_free(pmm_t *pmm, uint32_t frame);
uint32_t pmm_get_free_frames(pmm_t *pmm);

/* GDT - Global Descriptor Table */
void gdt_init(void);

/* IDT - Interrupt Descriptor Table */
void idt_init(void);

/* Paging - Virtual Memory */
void paging_init(void);
void paging_enable(void);

#endif /* __MEMORY_H__ */
