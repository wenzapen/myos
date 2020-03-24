#include "kmalloc.h"

static u32_t kmalloc_internal(u32_t size, int align, u32_t* phys) {
    if(align == 1 && (placement_address & 0xFFF) {
	placement_address &= 0xFFFFF000;
	placement_address += 0x1000;
    }
    if(phys) {
	*phys = placement_address;
    }
    u32_t tmp = placement_address;
    placement_address += size;
    return tmp;
}

u32_t kmalloc(u32_t size) {
    return kmalloc_internal(size, 0, 0);
}

u32_t kmalloc_a(u32_t size) {
    return kmalloc_internal(size, 1, 0);
}

u32_t kmalloc_p(u32_t size, u32_t* phys) {
    return kmalloc_internal(size, 0, phys);
}

u32_t kmalloc_ap(u32_t size, u32_t* phys) {
    return kmalloc_internal(size, 1, phys);
}
