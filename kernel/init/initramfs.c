#include "init/initramfs.h"

uint8_t *initramfs_in_ram = NULL;
struct dir_entry *root_entry = NULL;

static struct mount mount_table[MAX_MOUNTS];


static size_t hex_to_ulong(const char *hex, int len)
{
        size_t result = 0;
        for (int i = 0; i < len; i++) {
                char c = hex[i];
                result <<= 4;
                if (c >= '0' && c <= '9') {
                        result += c - '0';
                } else if (c >= 'A' && c <= 'F') {
                        result += c - 'A' + 10;
                } else if (c >= 'a' && c <= 'f') {
                        result += c - 'a' + 10;
                }
        }
        return result;
}

struct dir_entry *make_dir_entry(const char *name, int type, size_t size, void *data, struct dir_entry *parent)
{
        struct dir_entry *entry = kmalloc(sizeof(struct dir_entry));
        if (!entry) {
                panic("kmalloc failed in make_dir_entry\n");
        }

        entry->name = strdup(name);
        entry->type = type;
        entry->size = size;
        entry->data = data;
        entry->parent = parent;
        entry->child_count = 0;
        entry->child_capacity = 0;
        entry->children = NULL;

        return entry;
}

void add_child(struct dir_entry *parent, struct dir_entry *child)
{
        if (!parent) {
                return;
        }

        if (parent->child_capacity == 0) {
                parent->child_capacity = 4;
                parent->children = kmalloc(sizeof(struct dir_entry *) * parent->child_capacity);
                if (!parent->children) {
                        panic("kmalloc failed in add_child\n");
                }
        } else if (parent->child_count >= parent->child_capacity) {
                size_t new_cap = parent->child_capacity * 2;
                struct dir_entry **new_arr = kmalloc(sizeof(struct dir_entry *) * new_cap);

                if (!new_arr) {
                        panic("kmalloc failed in add_child (grow)\n");
                }

                for (size_t i = 0; i < parent->child_count; i++) {
                        new_arr[i] = parent->children[i];
                }
                parent->children = new_arr;
                parent->child_capacity = new_cap;
        }
        parent->children[parent->child_count++] = child;
}

struct file_in_ram find_file_in_initramfs(const char *path) {
        if (!initramfs_in_ram || !path) {
                struct file_in_ram empty = { NULL, 0 };
                return empty;
        }

        const char *search_path = path;
        if (search_path[0] == '/') {
                search_path++;
        }

        for (uint8_t *ptr = initramfs_in_ram;;) {
                struct cpio_newc_header *hdr = (struct cpio_newc_header *) ptr;

                if (memcmp(hdr->c_magic, "070701", 6) != 0) {
                        break;
                }

                size_t namesize = hex_to_ulong(hdr->c_namesize, 8);
                size_t filesize = hex_to_ulong(hdr->c_filesize, 8);

                char *name = (char *)(ptr + sizeof(struct cpio_newc_header));
                uint8_t *data = (unsigned char *)(ptr + sizeof(struct cpio_newc_header) + ALIGN4(namesize));

                if ((namesize == 2 && name[0] == '.') || (namesize >= 11 && strncmp(name, "TRAILER!!!", 10) == 0)) {
                        ptr += ALIGN4(sizeof(struct cpio_newc_header) + namesize + filesize);
                        continue;
                }

                size_t name_len = namesize - 1;

                if (strlen(search_path) == name_len && strncmp(name, search_path, name_len) == 0) {
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

        if (mount("/", initramfs.start, FS_TYPE_TMPFS) != 0) {
                panic("failed to mount initramfs at root\n");
        }
}

void parse_initramfs(void) {
        if (!initramfs_in_ram) {
                return;
        }

        root_entry = make_dir_entry("/", ENTRY_DIR, 0, NULL, NULL);

        for (uint8_t *ptr = initramfs_in_ram;;) {
                struct cpio_newc_header *hdr = (struct cpio_newc_header *) ptr;

                if (memcmp(hdr->c_magic, "070701", 6) != 0) {
                        break;
                }

                size_t namesize = hex_to_ulong(hdr->c_namesize, 8);
                size_t filesize = hex_to_ulong(hdr->c_filesize, 8);
                size_t mode = hex_to_ulong(hdr->c_mode, 8);

                char *name = (char *)(ptr + sizeof(struct cpio_newc_header));
                uint8_t *data = (uint8_t *)(ptr + sizeof(struct cpio_newc_header) + ALIGN4(namesize));

                if ((namesize == 2 && name[0] == '.') || (namesize >= 11 && strncmp(name, "TRAILER!!!", 10) == 0)) {
                        ptr += ALIGN4(sizeof(struct cpio_newc_header) + namesize + filesize);
                        continue;
                }

                struct dir_entry *cur = root_entry;
                char *p = name;
                while (*p) {
                        if (*p == '/') {
                                p++;
                                continue;
                        }

                        char part[128];
                        int i = 0;
                        while (*p && *p != '/' && i < (int)sizeof(part)-1) {
                                part[i++] = *p++;
                        }
                        part[i] = '\0';

                        struct dir_entry *found = NULL;
                        for (size_t j = 0; j < cur->child_count; j++) {
                                if (strcmp(cur->children[j]->name, part) == 0) {
                                        found = cur->children[j];
                                        break;
                                }
                        }

                        if (!found) {
                                int type;
                                if (*p) {
                                        type = ENTRY_DIR;
                                } else {
                                        type = S_ISDIR(mode) ? ENTRY_DIR : ENTRY_FILE;
                                }

                                void *filedata = (type == ENTRY_FILE) ? data : NULL;
                                size_t size = (type == ENTRY_FILE) ? filesize : 0;

                                found = make_dir_entry(part, type, size, filedata, cur);
                                add_child(cur, found);
                        }

                        cur = found;

                        while (*p == '/') {
                                p++;
                        }
                }

                ptr += ALIGN4(sizeof(struct cpio_newc_header) + namesize + filesize);
        }
}
void parse_initramfs_root() {
        if (!initramfs_in_ram) {
                return;
        }

        for (uint8_t *ptr = initramfs_in_ram;;) {
                struct cpio_newc_header *hdr = (struct cpio_newc_header *) ptr;

                if (memcmp(hdr->c_magic, "070701", 6) != 0) {
                        break;
                }

                size_t namesize = hex_to_ulong(hdr->c_namesize, 8);
                size_t filesize = hex_to_ulong(hdr->c_filesize, 8);
                char *name = (char *)(ptr + sizeof(struct cpio_newc_header));

                if (namesize >= 11 && strncmp(name, "TRAILER!!!", 10) == 0) {
                        break;
                }

                if (!(namesize == 2 && name[0] == '.')) {
                        kprintf("/");
                        for (size_t i = 0; i < namesize - 1; i++) {
                                kprintf("%c", name[i]);
                        }
                        kprintf("\n");
                }

                ptr += ALIGN4(sizeof(struct cpio_newc_header) + namesize + filesize);
        }
}


void initramfs_init(struct initramfs initramfs)
{
        if (!initramfs.start || initramfs.size == 0) {
                panic("initramfs is not loaded properly\n");
        }

        initramfs_in_ram = (uint8_t *) kmalloc(initramfs.size);
        if (!initramfs_in_ram) {
                panic("failed to allocate memory for initramfs\n");
        }

        for (size_t i = 0; i < initramfs.size; i++) {
                initramfs_in_ram[i] = initramfs.start[i];
        }

        if (mount("/", initramfs_in_ram, FS_TYPE_TMPFS) != 0) {
                panic("failed to mount initramfs at root\n");
        }

        parse_initramfs();
}

struct dir_entry *find_entry(const char *path) {
        if (!path || !root_entry) {
                return NULL;
        }

        if (strcmp(path, "/") == 0) {
                return root_entry;
        }

        struct dir_entry *dir = root_entry;
        const char *token = path;
        if (*token == '/') {
                token++;
        }
        char part[128];
        while (*token) {
                int len = 0;
                while (*token && *token != '/' && len < (int)(sizeof(part)-1)) {
                        part[len++] = *token++;
                }
                part[len] = '\0';
                while (*token == '/') {
                        token++;
                }

                struct dir_entry *next = NULL;
                if (dir->type != ENTRY_DIR) {
                        return NULL;
                }

                for (size_t i = 0; i < dir->child_count; i++) {
                        if (strcmp(dir->children[i]->name, part) == 0) {
                                next = dir->children[i];
                                break;
                        }
                }

                if (!next) {
                        return NULL;
                }
                dir = next;
        }

        return dir;
}

void print_dir(const char *path)
{
        struct dir_entry *dir = find_entry(path);

        if (!dir) {
                kprintf("No such directory: %s\n", path);
                return;
        }

        if (dir->type != ENTRY_DIR) {
                kprintf("%s is not a directory\n", path);
                return;
        }

        for (size_t i = 0; i < dir->child_count; i++) {
                struct dir_entry *child = dir->children[i];
                if (child->type == ENTRY_DIR) {
                        kprintf("%%l%s%%w\n", child->name);               
                } else if (child->type == ENTRY_FILE) {
                        kprintf("%s\n", child->name);        
                }
        }
}
