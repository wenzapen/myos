#ifnde INITRD_H
#define INITRD_H

#include "../common/types.h"
#include "fs.h"

typedef struct {
    u32_t nfiles; //the number of files in the ramdisk
} initrd_header_t;

typedef struct {
    u8_t magic;
    char name[64];
    u32_t offset;
    u32_t length;
} initrd_file_header_t;

//Initialise the initial ramdisk. it gets passed the address of the multiboot 
//module and returns a completed filesystem node.

fs_node_t *init_initrd(u32_t location);

#endif
