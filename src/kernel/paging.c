#include "paging.h"
#include "kheap.h"
#include "../common/types.h"
#include "../drivers/interrupt.h"
#include "../common/utils.h"
#include "../drivers/io.h"


u32_t *frames_bitmap;
u32_t nframes;
page_directory_t *kernel_directory=0;
page_directory_t *current_directory=0;

extern u32_t placement_address;
extern heap_t *kheap;
extern u32_t kernel_end;

extern void copy_page_physical(u32_t src_frame, u32_t dest_frame);


#define INDEX_FROM_BIT(n) (n/32)
#define OFFSET_FROM_BIT(n) (n%32)

static page_table_t *clone_table(page_table_t *src, u32_t *phys);


static void set_frame(u32_t frame_addr) {
    u32_t frame = frame_addr/0x1000;
    u32_t index = INDEX_FROM_BIT(frame);
    u32_t offset = OFFSET_FROM_BIT(frame);
    frames_bitmap[index] |= (0x1 << offset);
}

static void clear_frame(u32_t frame_addr) {
    u32_t frame = frame_addr/0x1000;
    u32_t index = INDEX_FROM_BIT(frame);
    u32_t offset = OFFSET_FROM_BIT(frame);
    frames_bitmap[index] &= ~(0x1 << offset);
}

static u32_t test_frame(u32_t frame_addr) {
    u32_t frame = frame_addr/0x1000;
    u32_t index = INDEX_FROM_BIT(frame);
    u32_t offset = OFFSET_FROM_BIT(frame);
    return (frames_bitmap[index] & (0x1 << offset));
}

static u32_t first_unused_frame() {
    u32_t i, j;
    for(i=0; i<INDEX_FROM_BIT(nframes); i++) {
	if(frames_bitmap[i] != 0xFFFFFFFF) {
	    for(j=0;j<32;j++) {
		if(!(frames_bitmap[i] & (0x1 << j))) {
			return i*32+j;
		}
	    }
	}
    }
}

void alloc_frame(page_t *page, int is_kernel, int is_writeable) {
    if(page->frame != 0) return;
    u32_t index = first_unused_frame();
    print_string("alloc frame for page: ");
    print_hex((u32_t)page);
    print_string(" frame index is: ");
    print_hex(index);
    print_char('\n'); 
    if(index==(u32_t)-1) {
	print_string("No free frames!\n");
	return;
    }
    set_frame(index*0x1000);
    page->present = 1;
    page->rw = is_writeable ? 1:0;
    page->user = is_kernel ? 0:1;
    page->frame = index;
}

static void free_frame(page_t *page) {
    u32_t frame = page->frame;;
    if(!frame) return;
    clear_frame(frame);
    page->frame = 0x0;
}

void init_paging() {
    u32_t mem_end_page = 0x1000000;
    nframes = mem_end_page/0x1000;

    frames_bitmap = (u32_t*)kmalloc(nframes/8);
    mem_set((u8_t *)frames_bitmap, 0, nframes/8);
    u32_t phys;
    kernel_directory = (page_directory_t*)kmalloc_a(sizeof(page_directory_t));
    mem_set((u8_t *)kernel_directory, 0, sizeof(page_directory_t));
//    current_directory = kernel_directory;
    kernel_directory->physical_addr = (u32_t)kernel_directory->tables_physical;

//setup page table for Kheap 
    u32_t h = KHEAP_START;
    if(h & 0x00000FFF) {
	h &= 0xFFFFF000;
    }
    while(h < KHEAP_START+KHEAP_INITIAL_SIZE+0x1000) {
	get_page(h,1,kernel_directory);
	h += 0x1000;
    }
    int i=0;
    while(i<placement_address + 0x1000) {
	alloc_frame(get_page(i,1,kernel_directory),0,0);
	i += 0x1000;
/*	print_string("address i: ");
	print_hex(i);
	print_string("\n");
*/    }
    h = KHEAP_START;
    if(h & 0x00000FFF) {
	h &= 0xFFFFF000;
    }
    while(h < KHEAP_START+KHEAP_INITIAL_SIZE+0x1000) {
	alloc_frame(get_page(h,1,kernel_directory),0,0);
	h += 0x1000;
    }
    register_interrupt_handler(14,&page_fault);
    switch_page_directory(kernel_directory);
    kheap = create_heap(KHEAP_START,KHEAP_START+KHEAP_INITIAL_SIZE,0xCFFFF000,0,0);
    switch_page_directory(current_directory);
}

void switch_page_directory(page_directory_t *dir) {
    current_directory = dir;
  asm volatile("mov %0,%%cr3"::"r"(dir->physical_addr));
    u32_t cr0;
    asm volatile("mov %%cr0, %0":"=r"(cr0));
    cr0 |= 0x80000000;
    asm volatile("mov %0, %%cr0"::"r"(cr0));
}

page_t* get_page(u32_t address, int make, page_directory_t *dir) {
    print_string("\n");
    print_string("get page index for address: ");
    print_hex(address);
    address /= 0x1000;
    u32_t table_index = address/1024;
    if(dir->page_tables[table_index]) {
	return &dir->page_tables[table_index]->pages[address%1024];
    } else if(make) {
	u32_t tmp;
	dir->page_tables[table_index] = (page_table_t*)kmalloc_ap(sizeof(page_table_t), &tmp);
	mem_set((u8_t *)dir->page_tables[table_index],0,0x1000);
	dir->tables_physical[table_index] = tmp|0x7;
    print_string(" page address is: ");
    print_hex((u32_t)&dir->page_tables[table_index]->pages[address%1024]);
    print_char('\n');
	return &dir->page_tables[table_index]->pages[address%1024];
	
    } else {
	return 0;
    }
}

void page_fault(registers_t regs) {
    u32_t fault_address;
    asm volatile("mov %%cr2, %0" : "=r"(fault_address));
    print_string("Page fault at : ");
    print_hex(fault_address);
    print_string("\n");
    while(1);

}

page_directory_t *clone_directory(page_directory_t *src) {
    u32_t phys;
    page_directory_t *dir = (page_directory_t *)kmalloc_ap(sizeof(page_directory_t), &phys);
    memset((u8_t *)dir, 0, sizeof(page_directory_t));
    u32_t offset = (u32_t)&dir->tables_physical - (u32_t)&dir;
    dir->physical_addr = phys + offset;

    //clone page_tables
    for(int i=0; i<1023; i++) {
	if(!src->page_tables[i])
	    continue;
	if(kernel_directory->page_tables[i] == src->page_tables[i]) {
	    dir->page_tables[i] = src->page_tables[i];
	    dir->tables_physical[i] = src->tables_physical[i]; 
	} else {
	    u32_t phys;
	    dir->page_tables[i] = clone_table(src->page_tables[i],&phys);
	    dir->tables_physical[i] = phys | 0x7;
	}
    }
    return dir;
}

static page_table_t *clone_table(page_table_t *src, u32_t *phys) {
    page_table_t *table = (page_table_t *)kmalloc_ap(sizeof(page_table_t),phys);
    memset((u8_t *)table, 0, sizeof(page_table_t));
    for(int i=0;i<1024;i++) {
	if(!src->pages[i].frame)
	    continue;
	alloc_frame(&table->pages[i],0,0);
	if(src->pages[i].present) table->pages[i].present=1;
	if(src->pages[i].rw) table->pages[i].rw=1;
	if(src->pages[i].user) table->pages[i].user=1;
	if(src->pages[i].accessed) table->pages[i].accessed=1;
	if(src->pages[i].dirty) table->pages[i].dirty=1;
	copy_page_physical(src->pages[i].frame*0x1000,table->pages[i].frame*0x1000);
    }
    return table;
}
