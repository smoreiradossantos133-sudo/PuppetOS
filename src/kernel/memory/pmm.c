#include <memory.h>
#include <types.h>

/* Helper functions for bitmap operations */
static void pmm_set_bit(uint8_t *bitmap, uint32_t bit) {
    uint32_t index = bit / 8;
    uint8_t offset = bit % 8;
    bitmap[index] |= (1 << offset);
}

static void pmm_clear_bit(uint8_t *bitmap, uint32_t bit) {
    uint32_t index = bit / 8;
    uint8_t offset = bit % 8;
    bitmap[index] &= ~(1 << offset);
}

static int pmm_test_bit(uint8_t *bitmap, uint32_t bit) {
    uint32_t index = bit / 8;
    uint8_t offset = bit % 8;
    return (bitmap[index] >> offset) & 1;
}

static uint32_t pmm_find_free_block(uint8_t *bitmap, uint32_t num_frames) {
    for (uint32_t i = 0; i < num_frames; i++) {
        if (!pmm_test_bit(bitmap, i)) {
            return i;
        }
    }
    return (uint32_t)-1;  /* No free frame */
}

void pmm_init(pmm_t *pmm, uint32_t total_frames) {
    pmm->bitmap = (uint8_t *)0x200000;  /* Place bitmap at 2MB */
    pmm->num_frames = total_frames;
    pmm->used_frames = 0;
    
    /* Clear all bits (mark all frames as free) */
    uint32_t bitmap_size = (total_frames + 7) / 8;
    for (uint32_t i = 0; i < bitmap_size; i++) {
        pmm->bitmap[i] = 0;
    }
}

uint32_t pmm_alloc_frame(pmm_t *pmm) {
    uint32_t frame = pmm_find_free_block(pmm->bitmap, pmm->num_frames);
    
    if (frame == (uint32_t)-1) {
        return (uint32_t)-1;  /* Out of memory */
    }
    
    pmm_set_bit(pmm->bitmap, frame);
    pmm->used_frames++;
    
    return frame;
}

void pmm_free_frame(pmm_t *pmm, uint32_t frame) {
    if (frame >= pmm->num_frames) {
        return;
    }
    
    if (pmm_test_bit(pmm->bitmap, frame)) {
        pmm_clear_bit(pmm->bitmap, frame);
        if (pmm->used_frames > 0) {
            pmm->used_frames--;
        }
    }
}

void pmm_mark_frame_used(pmm_t *pmm, uint32_t frame) {
    if (frame >= pmm->num_frames) {
        return;
    }
    
    if (!pmm_test_bit(pmm->bitmap, frame)) {
        pmm_set_bit(pmm->bitmap, frame);
        pmm->used_frames++;
    }
}

void pmm_mark_frame_free(pmm_t *pmm, uint32_t frame) {
    if (frame >= pmm->num_frames) {
        return;
    }
    
    if (pmm_test_bit(pmm->bitmap, frame)) {
        pmm_clear_bit(pmm->bitmap, frame);
        if (pmm->used_frames > 0) {
            pmm->used_frames--;
        }
    }
}

uint32_t pmm_get_free_frames(pmm_t *pmm) {
    return pmm->num_frames - pmm->used_frames;
}
