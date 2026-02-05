#include <stddef.h>
#include <stdint.h>

/* Simple bump allocator for early kernel use */
static uint8_t heap_region[1024*1024]; /* 1MB heap for kernel apps */
static size_t heap_offset = 0;

void *malloc(size_t size) {
    if (size == 0) return NULL;
    /* align to 8 */
    size = (size + 7) & ~7;
    if (heap_offset + size > sizeof(heap_region)) return NULL;
    void *p = &heap_region[heap_offset];
    heap_offset += size;
    return p;
}

void free(void *ptr) {
    /* noop for bump allocator */
    (void)ptr;
}

void *realloc(void *ptr, size_t size) {
    (void)ptr;
    return malloc(size);
}
