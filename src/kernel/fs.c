#include "fs.h"

fs_node_t *fs_root = 0;

u32_t read_fs(fs_node_t *node, u32_t offset, u32_t size, u8_t *buffer) {
    if(node->read != 0)
	return node->read(node, offset, size, buffer);
    return 0;
}

u32_t write_fs(fs_node_t *node, u32_t offset, u32_t size, u8_t *buffer) {
    if(node->write != 0)
	return node->write(node, offset, size, buffer);
    return 0;
}
void open_fs(fs_node_t *node, u8_t read, u8_t write) {
    if(node->open != 0) 
	return node->open(node, read, write);
    return 0;
}
void close_fs(fs_node_t *node) {
    if(node->close != 0) 
	return node->close(node);
    return 0;
}
struct dirent *readdir_fs(fs__node_t *node, u32_t index) {
   if((node->flags & 0x7) == FS_DIRECTORY && node->readdir != 0)
	return node->readdir(node, index)
    return 0; 
}
fs_node_t *finddir_fs(fs_node_t *node, char *name) {
   if((node->flags & 0x7) == FS_DIRECTORY && node->finddir != 0)
	return node->finddir(node, name)
    return 0; 
}
