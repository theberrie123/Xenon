#ifndef INITRAMFS_H
#define INITRAMFS_H

#include "xenon/string.h"
#include "xenon/type.h"
#include "xenon/memory.h"
#include "tty.h"

#define FS_TYPE_TMPFS 1
#define MAX_MOUNTS 16

#define ELF_MAGIC 0x464C457F
#define PT_LOAD 1

#define ENTRY_DIR  0
#define ENTRY_FILE 1

struct mount {
    char *mount_point;
    void *fs_data;
    int fs_type;
};

struct initramfs {
    unsigned char *start;
    unsigned char *end;
    unsigned long size;
};

struct file_in_ram {
    unsigned char *data;
    unsigned long size;
};

struct dir_entry {
    char *name;
    int type;                  // 0=dir, 1=file
    unsigned long size;
    void *data;
    struct dir_entry *parent;

    struct dir_entry **children;
    unsigned long child_count;
    unsigned long child_capacity; // <-- keep capacity so add_child can grow safely
};

struct cpio_newc_header {
    char c_magic[6];
    char c_ino[8];
    char c_mode[8];
    char c_uid[8];
    char c_gid[8];
    char c_nlink[8];
    char c_mtime[8];
    char c_filesize[8];
    char c_devmajor[8];
    char c_devminor[8];
    char c_rdevmajor[8];
    char c_rdevminor[8];
    char c_namesize[8];
    char c_check[8];
};

extern unsigned char *initramfs_in_ram;
extern struct dir_entry *root_entry;   /* root of parsed tree */

struct file_in_ram find_file_in_initramfs(const char *filename);

void initramfs_init(struct initramfs initramfs);

void parse_initramfs(void);

struct dir_entry *make_dir_entry(const char *name, int type, unsigned long size, void *data, struct dir_entry *parent);
void add_child(struct dir_entry *parent, struct dir_entry *child);

struct dir_entry *find_entry(const char *path);
void print_dir(const char *path);

void test(void);

#endif /* INITRAMFS_H */

