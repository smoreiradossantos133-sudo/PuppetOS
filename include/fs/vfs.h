/*
 * Virtual Filesystem Layer
 * Abstraction for different filesystem implementations
 * Inspired by Linux VFS
 */

#ifndef VFS_H
#define VFS_H

#include <kernel/kernel.h>

/* ===== INODE TYPES ===== */
typedef enum {
    INODE_TYPE_FILE,
    INODE_TYPE_DIRECTORY,
    INODE_TYPE_SYMLINK,
    INODE_TYPE_DEVICE,
    INODE_TYPE_SOCKET,
} inode_type_t;

/* ===== FILE PERMISSIONS ===== */
#define PERM_OWNER_READ    0x100
#define PERM_OWNER_WRITE   0x080
#define PERM_OWNER_EXEC    0x040
#define PERM_GROUP_READ    0x020
#define PERM_GROUP_WRITE   0x010
#define PERM_GROUP_EXEC    0x008
#define PERM_OTHER_READ    0x004
#define PERM_OTHER_WRITE   0x002
#define PERM_OTHER_EXEC    0x001

/* ===== FILE DESCRIPTOR ===== */
typedef struct {
    uint32_t fd;
    void *inode;
    uint64_t offset;
    uint32_t flags;
    uint32_t permissions;
} file_descriptor_t;

/* ===== INODE STRUCTURE ===== */
typedef struct inode {
    uint64_t inode_number;
    inode_type_t type;
    uint32_t permissions;
    uid_t uid;
    gid_t gid;
    uint64_t size;
    uint64_t created_time;
    uint64_t modified_time;
    uint64_t accessed_time;
    uint32_t hard_links;
    void *filesystem;  /* Pointer to parent filesystem */
    void *private_data;
} inode_t;

/* ===== VFS OPERATIONS ===== */
typedef struct {
    inode_t *(*lookup)(inode_t *parent, const char *name);
    int (*create)(inode_t *parent, const char *name, inode_type_t type);
    int (*mkdir)(inode_t *parent, const char *name);
    int (*rmdir)(inode_t *parent, const char *name);
    int (*unlink)(inode_t *parent, const char *name);
    ssize (*read)(inode_t *inode, uint64_t offset, void *buffer, size_t size);
    ssize (*write)(inode_t *inode, uint64_t offset, const void *buffer, size_t size);
} vfs_ops_t;

/* ===== VFS MAIN FUNCTIONS ===== */
int vfs_init(void);
inode_t *vfs_lookup_path(const char *path);
inode_t *vfs_create_file(const char *path);
int vfs_mount(const char *mount_point, void *filesystem);

#endif /* VFS_H */
