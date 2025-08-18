#ifndef FS_H
#define FS_H



#include "../include/type.h"
#include "../include/string.h"
#include "../include/tty.h"




#define EXT4_BLOCK_SIZE 4096
#define EXT4_INODE_SIZE 128
#define MAX_BLOCKS 1024
#define MAX_INODES 128
#define MAX_FILENAME 255

typedef struct {
    uint32_t i_size;        // Size in bytes
    uint32_t i_block[12];   // Direct blocks only
    uint8_t i_type;         // 0=file, 1=dir
} ext4_inode_t;

typedef struct {
    uint32_t inode;
    char name[MAX_FILENAME+1];
} ext4_dir_entry_t;

extern uint8_t disk[MAX_BLOCKS][EXT4_BLOCK_SIZE];
extern uint8_t block_bitmap[MAX_BLOCKS/8];
extern uint8_t inode_bitmap[MAX_INODES/8];
extern ext4_inode_t inode_table[MAX_INODES];

int find_free_bit(uint8_t *bitmap, int size);

void set_bit(uint8_t *bitmap, int idx);
int alloc_inode();

int alloc_block();


int create_dir(int parent_inode, const char *name);

extern int root_inode;

void init_fs();
void list_dir(int inode_idx);

#endif
