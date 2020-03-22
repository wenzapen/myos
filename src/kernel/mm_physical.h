#ifndef MM_PHYSICAL_H
#define MM_PHYSICAL_H
#include "../common/types.h"

typedef u32_t physical_addr;

void init_mm_physical(u32_t mem_size, physical_addr bitmap);

#endif
