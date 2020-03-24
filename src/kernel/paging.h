#ifndef PAGING_H
#define PAGING_H

#include "../common/types.h"

typedef struct page {
    u32_t present : 1;
    u32_t rw : 1;
    u32_t user : 1;
    u32_t accessed : 1;
    u32_t dirty : 1;
    u32_t unused : 7;
    u32_t frame : 20;
} page_t;

typedef struct page_table {
    page_t pages[1024];
} page_table_t;

typedef struct page_directory {
    page_table_t* page_tables[1024];
    u32_t tables_physical[1024];
    u32_t physical_addr;
} page_directory_t;

void init_paging();
void switch_page_directory(page_directory_t *new);
page_t *get_page(u32_t address, int make, page_directory_t *dir);
void page_fault(registers_t regs);



#endif
