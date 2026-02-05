/*
 * Process Management Implementation
 * Handles process creation, scheduling, and management
 */

#include <kernel/process.h>
#include <kernel/kernel.h>
#include <stddef.h>
#include <string.h>
#include <stdlib.h>

/* ===== PROCESS TABLE ===== */
#define MAX_PROCESSES 256

static process_t *process_table[MAX_PROCESSES];
static uint32_t num_processes = 0;
static kpid_t next_pid = 1;
static spinlock_t process_table_lock;

static process_t *current_process = NULL;

/* ===== PROCESS CREATION ===== */
kpid_t process_create(const char *name, vaddr_t entry_point, uid_t uid) {
    spinlock_acquire(&process_table_lock);
    
    if (num_processes >= MAX_PROCESSES) {
        spinlock_release(&process_table_lock);
        return -1;  /* Error: Process table full */
    }
    
    process_t *proc = (process_t *)malloc(sizeof(process_t));
    if (!proc) {
        spinlock_release(&process_table_lock);
        return -1;
    }
    
    /* Initialize process */
    proc->pid = next_pid++;
    proc->uid = uid;
    proc->gid = 0;
    
    strncpy(proc->name, name, 255);
    proc->name[255] = '\0';
    
    proc->state = PROCESS_STATE_CREATED;
    proc->code_start = entry_point;
    proc->cpu_ticks = 0;
    proc->creation_time = 0;  /* TODO: Get current time */
    proc->exit_code = 0;
    
    proc->children = NULL;
    proc->num_children = 0;
    proc->parent_pid = 0;
    
    /* Add to process table */
    process_table[num_processes++] = proc;
    
    KINFO("Process created: %s (PID %d)", name, proc->pid);
    
    spinlock_release(&process_table_lock);
    return proc->pid;
}

/* ===== PROCESS EXIT ===== */
void process_exit(kpid_t pid, int exit_code) {
    spinlock_acquire(&process_table_lock);
    
    for (uint32_t i = 0; i < num_processes; i++) {
        if (process_table[i]->pid == pid) {
            process_table[i]->state = PROCESS_STATE_TERMINATED;
            process_table[i]->exit_code = exit_code;
            
            KINFO("Process exited: %s (PID %d, code %d)", 
                  process_table[i]->name, pid, exit_code);
            
            /* TODO: Clean up resources */
            break;
        }
    }
    
    spinlock_release(&process_table_lock);
}

/* ===== PROCESS LOOKUP ===== */
process_t *process_get_current(void) {
    return current_process;
}

process_t *process_get_by_pid(kpid_t pid) {
    if (pid == 0) return current_process;
    
    for (uint32_t i = 0; i < num_processes; i++) {
        if (process_table[i]->pid == pid) {
            return process_table[i];
        }
    }
    
    return NULL;
}

/* ===== PROCESS LISTING ===== */
void process_list_all(void) {
    KINFO("=== Process Table ===");
    KINFO("Count: %d", num_processes);
    
    for (uint32_t i = 0; i < num_processes; i++) {
        process_t *p = process_table[i];
        KINFO("  [%d] %s (UID %d, state %d)", p->pid, p->name, p->uid, p->state);
    }
}

/* ===== SCHEDULER ===== */
static uint32_t scheduler_current = 0;

void scheduler_init(void) {
    KINFO("Scheduler initialized");
    
    /* Create idle process */
    process_create("idle", 0, 0);
}

void scheduler_tick(void) {
    if (current_process) {
        current_process->cpu_ticks++;
    }
}

void scheduler_switch(void) {
    spinlock_acquire(&process_table_lock);
    
    if (num_processes == 0) {
        spinlock_release(&process_table_lock);
        return;
    }
    
    /* Simple round-robin scheduling */
    do {
        scheduler_current = (scheduler_current + 1) % num_processes;
    } while (process_table[scheduler_current]->state != PROCESS_STATE_READY && 
             process_table[scheduler_current]->state != PROCESS_STATE_CREATED);
    
    current_process = process_table[scheduler_current];
    current_process->state = PROCESS_STATE_RUNNING;
    
    spinlock_release(&process_table_lock);
}

process_t *scheduler_next_process(void) {
    scheduler_switch();
    return current_process;
}

/* ===== IDLE PROCESS ===== */
void idle_process_entry(void) {
    while (1) {
        asm volatile("hlt");
    }
}
