#include "init/initramfs.h"

unsigned char *initramfs_in_ram = NULL;

static struct mount mount_table[MAX_MOUNTS];


#define ALIGN4(x) (((x) + 3) & ~3)

struct file_in_ram find_file_in_initramfs(const char *path) {
    if (!initramfs_in_ram || !path) {
        struct file_in_ram empty = { NULL, 0 };
        return empty;
    }

    const char *search_path = path;
    if (search_path[0] == '/') search_path++;

    unsigned char *ptr = initramfs_in_ram;

    while (1) {
        struct cpio_newc_header *hdr = (struct cpio_newc_header *) ptr;

        if (memcmp(hdr->c_magic, "070701", 6) != 0) break;

        unsigned long namesize = hex_to_ulong(hdr->c_namesize, 8);
        unsigned long filesize = hex_to_ulong(hdr->c_filesize, 8);

        char *name = (char *)(ptr + sizeof(struct cpio_newc_header));
        unsigned char *data = (unsigned char *)(ptr + sizeof(struct cpio_newc_header) + ALIGN4(namesize));

        if ((namesize == 2 && name[0] == '.') ||
            (namesize >= 11 && strncmp(name, "TRAILER!!!", 10) == 0)) {
            ptr += ALIGN4(sizeof(struct cpio_newc_header) + namesize + filesize);
            continue;
        }

        unsigned long name_len = namesize - 1;

        if (strlen(search_path) == name_len &&
            strncmp(name, search_path, name_len) == 0) {
            struct file_in_ram file = { data, filesize };
            return file;
        }

        ptr += ALIGN4(sizeof(struct cpio_newc_header) + namesize + filesize);
    }

    struct file_in_ram empty = { NULL, 0 };
    return empty;
}


int add_mount(const char *mount_point, void *fs_data, int fs_type) {
    for (int i = 0; i < MAX_MOUNTS; i++) {
        if (mount_table[i].mount_point == NULL) {
            mount_table[i].mount_point = (char *)mount_point;
            mount_table[i].fs_data = fs_data;
            mount_table[i].fs_type = fs_type;
            return 0;
        }
    }
    return -1;
}

int mount(const char *mount_point, void *source, int fs_type) {
        if (!mount_point || !source) {
                panic("invalid mount point or source\n");
    }

    switch (fs_type) {
        case FS_TYPE_TMPFS:
            if (add_mount((char *)mount_point, source, fs_type) == 0) {
                kprintf("[  %%gOK%%w  ]  tmpfs mounted at %s\n", mount_point);
                return 0;
            }
            break;

        default:
                panic("unknown file system type: %d\n", fs_type);
    }

    return -1;
}

void init_mount(struct initramfs initramfs) {
        if (!initramfs.start || initramfs.size == 0) {
                panic("initramfs is not loaded properly. start: %d, size: %d", initramfs.start, initramfs.size);
        }

        kprintf("[  %%gOK%%w  ]  Initializing initramfs\n");

        if (mount("/", initramfs.start, FS_TYPE_TMPFS) != 0) {
                panic("failed to mount initramfs at root\n");
        } else {
                kprintf("[  %%gOK%%w  ]  initramfs successfully mounted at root (/)\n");
        }
}

void parse_initramfs_root(void) {
    if (!initramfs_in_ram) return;

    unsigned char *ptr = initramfs_in_ram;
    while (1) {
        struct cpio_newc_header *hdr = (struct cpio_newc_header *) ptr;
        if (memcmp(hdr->c_magic, "070701", 6) != 0) break;

        unsigned long namesize = hex_to_ulong(hdr->c_namesize, 8);
        unsigned long filesize = hex_to_ulong(hdr->c_filesize, 8);
        char *name = (char *)(ptr + sizeof(struct cpio_newc_header));

        if ((namesize == 2 && name[0] == '.') ||
            (namesize >= 11 && strncmp(name, "TRAILER!!!", 10) == 0)) {
            ptr += ALIGN4(sizeof(struct cpio_newc_header) + namesize + filesize);
            continue;
        }

        kprintf("/");
        for (unsigned long i = 0; i < namesize - 1; i++)
            kprintf("%c", name[i]);
        kprintf("\n");

        ptr += ALIGN4(sizeof(struct cpio_newc_header) + namesize + filesize);
    }
}

void unpack_to_rootfs(struct initramfs initramfs) {
    if (!initramfs.start || initramfs.size == 0) {
        kprintf("[  %%rERR%%w ] initramfs is not loaded properly\n");
        return;
    }

    initramfs_in_ram = (unsigned char *) kmalloc(initramfs.size);
    if (!initramfs_in_ram) {
        kprintf("[  %%rERR%%w ] Failed to allocate memory for initramfs\n");
        return;
    }

    for (unsigned long i = 0; i < initramfs.size; i++)
        initramfs_in_ram[i] = initramfs.start[i];

    kprintf("[  %%gOK%%w  ]  initramfs loaded into RAM\n");

    if (mount("/", initramfs_in_ram, FS_TYPE_TMPFS) != 0) {
        kprintf("[  %%rERR%%w ] Failed to mount initramfs at root\n");
        return;
    }
    kprintf("[  %%gOK%%w  ]  initramfs mounted at root (/)\n");

    kprintf("Parsing initramfs root directory:\n");
    parse_initramfs_root();
}

