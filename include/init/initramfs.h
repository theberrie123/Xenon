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

static unsigned long align4(unsigned long x) {
    return (x + 3) & ~3;
}

static unsigned long hex_to_ulong(const char *hex, int len)
{
        unsigned long result = 0;
        for (int i = 0; i < len; i++) {
                char c = hex[i];
                result <<= 4;
                if (c >= '0' && c <= '9') result += c - '0';
                else if (c >= 'A' && c <= 'F') result += c - 'A' + 10;
                else if (c >= 'a' && c <= 'f') result += c - 'a' + 10;
        }
        return result;
}

extern unsigned char *initramfs_in_ram;


struct file_in_ram find_file_in_initramfs(const char *filename);


void unpack_to_rootfs(struct initramfs initramfs);


#endif /* INITRAMFS_H */
