#ifndef GDT_H
#define GDT_H
#include "../common/types.h"

struct gdt_struct {
    u16_t limit;
    u32_t base;
} __attribute__((packed));

typedef struct gdt_struct gdt_t;

struct gdt_entry_struct {
    u16_t limit_low;
    u16_t base_low;
    u8_t base_middle;
    u8_t access;
    u8_t granularity;
    u8_t base_high;
} __attribute__((packed));

typedef struct gdt_entry_struct gdt_entry_t;

void init_gdt();    

#endif
