#ifndef PAGING_H
#define PAGING_H

#include "../common/types.h"
#include "../drivers/interrupt.h"

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
    page_table_t* page_tables[1024]; //pointers to pagetables
    u32_t tables_physical[1024]; //physical address of pagetables
    u32_t physical_addr; //physical address of tables_physical
} page_directory_t;

void init_paging();
void switch_page_directory(page_directory_t *new);
page_t *get_page(u32_t address, int make, page_directory_t *dir);
void page_fault(registers_t regs);
void alloc_frame(page_t *page, int is_kernel, int is_writeable);
page_directory_t *clone_directory(page_directory_t *src);
void print_serial_tables(page_directory_t *dir);
u32_t get_physical_address(u32_t virtual_address, page_directory_t *dir); 

#endif
