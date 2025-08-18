#include "../include/type.h"
#include "../include/string.h"
#include "../include/tty.h"
#include "../include/fs.h"


uint8_t disk[MAX_BLOCKS][EXT4_BLOCK_SIZE];
uint8_t block_bitmap[MAX_BLOCKS/8];
uint8_t inode_bitmap[MAX_INODES/8];
ext4_inode_t inode_table[MAX_INODES];
int root_inode;


int find_free_bit(uint8_t *bitmap, int size) {
    for(int i=0;i<size*8;i++)
        if(!(bitmap[i/8] & (1<<(i%8)))) return i;
    return -1;
}

void set_bit(uint8_t *bitmap, int idx) {
    bitmap[idx/8] |= (1<<(idx%8));
}


int alloc_inode() {
    int idx = find_free_bit(inode_bitmap, MAX_INODES/8);
    if(idx>=0) set_bit(inode_bitmap, idx);
    return idx;
}

int alloc_block() {
    int idx = find_free_bit(block_bitmap, MAX_BLOCKS/8);
    if(idx>=0) set_bit(block_bitmap, idx);
    return idx;
}

int create_dir(int parent_inode, const char *name) {
    int inode_idx = alloc_inode();
    if(inode_idx < 0) return -1;

    ext4_inode_t *inode = &inode_table[inode_idx];
    inode->i_size = 0;
    inode->i_type = 1; // directory

    // Allocate one block for directory entries
    int blk = alloc_block();
    if(blk<0) return -1;
    inode->i_block[0] = blk;

    // Add entry to parent
    if(parent_inode>=0) {
        ext4_inode_t *parent = &inode_table[parent_inode];
        ext4_dir_entry_t *entries = (ext4_dir_entry_t*)disk[parent->i_block[0]];
        entries[parent->i_size++] = (ext4_dir_entry_t){ .inode = inode_idx };
        strncpy(entries[parent->i_size-1].name, name, MAX_FILENAME);
    }

    return inode_idx;
}



int create_file(int parent_inode, const char *name, uint8_t *data, uint32_t size) {
    int inode_idx = alloc_inode();
    if(inode_idx<0) return -1;

    ext4_inode_t *inode = &inode_table[inode_idx];
    inode->i_size = size;
    inode->i_type = 0; // file

    int blocks_needed = (size+EXT4_BLOCK_SIZE-1)/EXT4_BLOCK_SIZE;
    if(blocks_needed>12) return -1;

    for(int i=0;i<blocks_needed;i++){
        int blk = alloc_block();
        if(blk<0) return -1;
        inode->i_block[i] = blk;
        uint32_t copy_size = (size>EXT4_BLOCK_SIZE)? EXT4_BLOCK_SIZE : size;
        memcpy(disk[blk], data+i*EXT4_BLOCK_SIZE, copy_size);
        size -= copy_size;
    }

    // Add entry to parent directory
    ext4_inode_t *parent = &inode_table[parent_inode];
    ext4_dir_entry_t *entries = (ext4_dir_entry_t*)disk[parent->i_block[0]];
    entries[parent->i_size++] = (ext4_dir_entry_t){ .inode = inode_idx };
    strncpy(entries[parent->i_size-1].name, name, MAX_FILENAME);

    return inode_idx;
}

void list_dir(int inode_idx)
{
        ext4_inode_t *dir = &inode_table[inode_idx];
        
        /* not a dir */
        if (dir->i_type != 1) {
                kprintf("Not a directory\n");
                return;
        }

        ext4_dir_entry_t *entries = (ext4_dir_entry_t*)disk[dir->i_block[0]];
        kprintf("Directory contents (%d entries):\n", dir->i_size);

        for (uint32_t i = 0; i < dir->i_size; i++) {
                ext4_inode_t *e_inode = &inode_table[entries[i].inode];
                if (e_inode->i_type == 1) {
                        kprintf("[DIR]  %s\n", entries[i].name);
                } else {
                        kprintf("[FILE] %s\n", entries[i].name);
                }
        }
}

void init_root_fs() {
    memset(disk, 0, sizeof(disk));
    memset(block_bitmap, 0, sizeof(block_bitmap));
    memset(inode_bitmap, 0, sizeof(inode_bitmap));
    memset(inode_table, 0, sizeof(inode_table));

    root_inode = create_dir(-1, "/"); // root
    create_dir(root_inode, "bin");
    create_dir(root_inode, "dev");
    create_dir(root_inode, "etc");
    create_dir(root_inode, "home");

    // Add a test file
    uint8_t data[1024]; for(int i=0;i<1024;i++) data[i]=i%256;
    create_file(root_inode, "hello.txt", data, 1024);
}
