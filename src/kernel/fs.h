#ifndef FS_H
#define FS_H

#include "../common/types.h"

#define FS_FILE 0x01
#define FS_DIRECTORY 0x02
#define FS_CHARDEVICE 0x03
#define FS_BLOCKDEVICE 0x04
#define FS_PIPE 0x05
#define FS_SYMLINK 0x06
#define FS_MOUNTPOINT 0x08

struct fs_node;

typedef u32_t (*read_type_t)(struct fs_node*,u32_t, u32_t, u8_t*);
typedef u32_t (*write_type_t)(struct fs_node*,u32_t, u32_t, u8_t*);
typedef void (*open_type_t)(struct fs_node*);
typedef void (*close_type_t)(struct fs_node*);
typedef struct dirent* (*readdir_type_t)(struct fs_node*, u32_t);
typedef struct fs_node* (*finddir_type_t)(struct fs_node*, char *name);

typedef struct fs_node {
    char name[128];
    u32_t mask;
    u32_t uid;
    u32_t gid;
    u32_t flags;
    u32_t inode;
    u32_t length;
    u32_t impl;
    read_type_t read;
    write_type_t write;
    open_type_t open;
    close_type_t close;
    readdir_type_t readdir;
    finddir_type_t finddir;
    struct fs_node *ptr;    
} fs_node_t;


struct dirent {
    char name[128];
    u32_t ino;
};


extern fs_node_t *fs_root;

u32_t read_fs(fs_node_t *node, u32_t offset, u32_t size, u8_t *buffer);
u32_t write_fs(fs_node_t *node, u32_t offset, u32_t size, u8_t *buffer);
void open_fs(fs_node_t *node, u8_t read, u8_t write);
void close_fs(fs_node_t *node);
struct dirent *readdir_fs(fs_node_t *node, u32_t index);
fs_node_t *finddir_fs(fs_node_t *node, char *name);

#endif
