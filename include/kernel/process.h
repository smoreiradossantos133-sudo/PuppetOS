/*
 * Process/Task Management
 * Handles multitasking and process scheduling
 */

#ifndef PROCESS_H
#define PROCESS_H

#include <kernel/kernel.h>

/* ===== PROCESS STATES ===== */
typedef enum {
    PROCESS_STATE_CREATED,
    PROCESS_STATE_READY,
    PROCESS_STATE_RUNNING,
    PROCESS_STATE_WAITING,
    PROCESS_STATE_STOPPED,
    PROCESS_STATE_TERMINATED,
} process_state_t;

/* ===== PROCESS STRUCTURE ===== */
typedef struct process {
    kpid_t pid;
    uid_t uid;
    gid_t gid;
    
    char name[256];
    process_state_t state;
    
    vaddr_t code_start;
    vaddr_t code_end;
    vaddr_t data_start;
    vaddr_t data_end;
    vaddr_t heap_start;
    vaddr_t heap_end;
    vaddr_t stack_start;
    vaddr_t stack_end;
    
    uint64_t cpu_ticks;
    uint64_t creation_time;
    uint64_t exit_code;
    
    void *page_directory;  /* Virtual->Physical mapping */
    
    // File descriptors
    void *open_files[256];
    
    // Child processes
    kpid_t *children;
    size_t num_children;
    kpid_t parent_pid;
    
} process_t;

/* ===== PROCESS MANAGEMENT ===== */
kpid_t process_create(const char *name, vaddr_t entry_point, uid_t uid);
void process_exit(kpid_t pid, int exit_code);
process_t *process_get_current(void);
process_t *process_get_by_pid(kpid_t pid);
void process_list_all(void);

/* ===== SCHEDULER ===== */
void scheduler_init(void);
void scheduler_tick(void);
void scheduler_switch(void);
process_t *scheduler_next_process(void);

/* ===== IDLE PROCESS ===== */
void idle_process_entry(void);

#endif /* PROCESS_H */
