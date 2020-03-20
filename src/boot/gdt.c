#include "gdt.h"

extern void load_gdt(u32_t);
gdt_t gdt;
gdt_entry_t gdt_entries[5];

static void gdt_set_gate(u8_t n,u32_t base,u32_t limit,u8_t access,u8_t gran) {
    gdt_entries[n].base_low = base & 0xffff;
    gdt_entries[n].base_middle = (base >> 16) & 0xff;
    gdt_entries[n].base_high = (base >> 24) & 0xff;

    gdt_entries[n].limit_low = limit & 0xffff;
    gdt_entries[n].granularity = (limit >> 16) & 0x0f;
    gdt_entries[n].granularity |= gran & 0xf0;
    gdt_entries[n].access = access;


}

void init_gdt() {
    gdt.base = (u32_t)&gdt_entries;
    gdt.limit = sizeof(gdt_entry_t) * 5 -1;
    
    gdt_set_gate(0,0,0,0,0);
    gdt_set_gate(1,0,0xffffffff,0x9a,0xcf);
    gdt_set_gate(2,0,0xffffffff,0x92,0xcf);
    gdt_set_gate(3,0,0xffffffff,0xfa,0xcf);
    gdt_set_gate(4,0,0xffffffff,0xf2,0xcf);
    load_gdt((u32_t)&gdt);
}
