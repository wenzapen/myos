#include "paging.h"

u32_t *frames_bitmap;
u32_t nframes;

extern u32_t placement_address;

#define INDEX_FROM_BIT(n) (n/32)
#define OFFSET_FROM_BIT(n) (n%32)

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

static void alloc_frame(page_t *page, int is_kernel, int is_writeable) {
    if(page->frame != 0) return;
    u32_t index = first_unused_frame();
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
    mem_set(frames_bitmap, 0, nframes/8);

    kernel_directory = (page_directory_t*)kmalloc_a(sizeof(page_directory_t));
    mem_set(kernel_directory, 0, sizeof(page_directory_t));
    current_directory = kernel_directory;
    
    int i=0;
    while(i<placement_address) {
	alloc_frame(get_page(i,1,kernel_directory),0,0);
	i += 0x1000;
    }
    register_interrupt_handler(14, page_fault);
    switch_page_directory(kernel_directory);

}

void switch_page_directory(page_directory_t *dir) {
    current_directory = dir;
    asm volatile("mov %0,%%cr3"::"r"(&dir->tables_physical));
    u32_t cr0;
    asm volatile("mov %%cr0, %0":"=r"(cr0));
    cr0 |= 0x80000000;
    asm volatile("mov %0, %%cr0"::"r"(cr0));
}

page_t* get_page(u32_t address, int make, page_directory_t *dir) {
    address /= 0x1000;
    u32_t table_index = address/1024;
    if(dir->tables[table_index]) {
	return &dir->tables[table_index]->pages[address%1024];
    } else if(make) {
	u32_t tmp;
	dir->tables[table_index] = (page_table_t*)kmalloc_ap(sizeof(page_table_t), &tmp);
	mem_set(dir->tables[table_index],0,0x1000);
	dir->tables_physical[table_index] = tmp|0x7;
	return &dir->tables[table_index]->pages[address%1024];
	
    } else {
	return 0;
    }
}

void page_fault(register_t regs) {
    u32_t fault_address;
    asm volatile("mov %%cr2, %0" : "=r"(fault_address));
    print_string("Page fault at : ");
    print_hex(fault_address);
    print_string("\n");

}
