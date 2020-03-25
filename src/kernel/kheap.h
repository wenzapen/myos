#ifndef KMALLOC_H
#define KMALLOC_H

#include "../common/types.h"

u32_t kmalloc_a(u32_t size); //page aligned
u32_t kmalloc_p(u32_t size, u32_t* phys); //return a physical address
u32_t kmalloc_ap(u32_t size, u32_t* phys); // page aligned and return phy addr
u32_t kmalloc(u32_t size); //normal 



#endif

