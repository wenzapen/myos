#ifndef KMALLOC_H
#define KMALLOC_H

#include "../common/types.h"

#define KHEAP_START 0xc0000000
#define KHEAP_INITIAL_SIZE 0x100000
#define HEAP_INDEX_SIZE 0x20000
#define HEAP_MAGIC 0x123890AB
#define HEAP_MIN_SIZE 0x70000

typedef struct {
    u32_t magic;
    u8_t is_hole;
    u32_t size;
} header_t;

typedef struct {
    u32_t magic;
    header_t *header;
} footer_t;

typedef struct {
    ordered_array_t index;
    u32_t start_address;
    u32_t end_address;
    u32_t max_address;
    u8_t supervisor;
    u8_t readonly;
} heap_t;

heap_t *create_heap(u32_t start, u32_t end, u32_t max, u8_t supervisor, u8_t readonly);
void *alloc(u32_t size, u8_t page_align, heap_t *heap);
void free(void *p, heap_t *heap);

u32_t kmalloc_a(u32_t size); //page aligned
u32_t kmalloc_p(u32_t size, u32_t* phys); //return a physical address
u32_t kmalloc_ap(u32_t size, u32_t* phys); // page aligned and return phy addr
u32_t kmalloc(u32_t size); //normal 



#endif

