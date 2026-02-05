/*
 * PuppetOS Kernel Architecture
 * Central header with core data structures and definitions
 * Compatible with Windows 7-style OS design
 */

#ifndef KERNEL_H
#define KERNEL_H

#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>

/* ===== KERNEL VERSION ===== */
#define PUPPETOS_VERSION_MAJOR 0
#define PUPPETOS_VERSION_MINOR 4
#define PUPPETOS_VERSION_PATCH 0

#define PUPPETOS_BUILD_DATE __DATE__
#define PUPPETOS_BUILD_TIME __TIME__

/* ===== FUNDAMENTAL TYPES ===== */
typedef uint64_t usize;
typedef int64_t ssize;
typedef uint64_t vaddr_t;  /* Virtual address */
typedef uint64_t paddr_t;  /* Physical address */
typedef uint64_t kpid_t;    /* Process ID (kernel namespace) */
typedef uint64_t tid_t;    /* Thread ID */
typedef uint32_t uid_t;    /* User ID */
typedef uint32_t gid_t;    /* Group ID */

/* ===== KERNEL PANIC ===== */
void kernel_panic(const char *format, ...) __attribute__((noreturn));
void kernel_warn(const char *format, ...);
void kernel_log(const char *level, const char *format, ...);

#define KPANIC(fmt, ...) kernel_panic("[PANIC] " fmt, ##__VA_ARGS__)
#define KWARN(fmt, ...)  kernel_warn("[WARN] " fmt, ##__VA_ARGS__)
#define KINFO(fmt, ...)  kernel_log("INFO", fmt, ##__VA_ARGS__)
#define KDEBUG(fmt, ...) kernel_log("DEBUG", fmt, ##__VA_ARGS__)

/* ===== KERNEL STATE ===== */
typedef enum {
    KERNEL_STATE_BOOTING,
    KERNEL_STATE_RUNNING,
    KERNEL_STATE_SHUTTING_DOWN,
    KERNEL_STATE_PANIC,
} kernel_state_t;

extern kernel_state_t kernel_state;

/* ===== BOOT INFORMATION ===== */
struct boot_info {
    uint64_t memory_size;
    uint64_t usable_memory;
    uint32_t num_cpus;
    vaddr_t framebuffer;
    uint32_t framebuffer_width;
    uint32_t framebuffer_height;
    uint32_t framebuffer_pitch;
    uint32_t framebuffer_bpp;
};

extern struct boot_info bootinfo;

/* ===== SPINLOCK (Basic synchronization) ===== */
typedef struct {
    volatile uint32_t locked;
} spinlock_t;

static inline void spinlock_init(spinlock_t *lock) {
    lock->locked = 0;
}

static inline void spinlock_acquire(spinlock_t *lock) {
    while (__atomic_test_and_set(&lock->locked, __ATOMIC_ACQUIRE)) {
        __asm__ volatile("pause");
    }
}

static inline void spinlock_release(spinlock_t *lock) {
    __atomic_clear(&lock->locked, __ATOMIC_RELEASE);
}

/* ===== ASSERT MACROS ===== */
#define KASSERT(cond) \
    do { \
        if (!(cond)) { \
            KPANIC("Assertion failed: %s, file %s, line %d", #cond, __FILE__, __LINE__); \
        } \
    } while (0)

#endif /* KERNEL_H */
