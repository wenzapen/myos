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

void alloc_frame(page_t *page, int is_kernel, int is_writeable) {
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
    mem_set((u8_t *)frames_bitmap, 0, nframes/8);

    kernel_directory = (page_directory_t*)kmalloc_a(sizeof(page_directory_t));
    mem_set((u8_t *)kernel_directory, 0, sizeof(page_directory_t));
    current_directory = kernel_directory;
    print_string("allocate kernal_directory: ");
    print_string("\n");
    
    int i=0;
    while(i<placement_address) {
//	alloc_frame(get_page(i,1,kernel_directory),0,0);
	i += 0x1000;
    print_string("allocating frame, kernel_directory is : ");
    print_hex((u32_t)kernel_directory);
    print_string("\n");
    }
    print_string("start to register page-fault\n");
    register_interrupt_handler(14,&page_fault);
    
    print_string("starting to enable page, kernel_directory is : ");
    print_hex((u32_t)kernel_directory);
    print_string("\n");
    switch_page_directory(kernel_directory);
    print_string("finished enabling page\n");
    kheap = create_heap(KHEAP_START,KHEAP_START+KHEAP_INITIAL_SIZE,0xCFFFF000,0,0);
}

void switch_page_directory(page_directory_t *dir) {
    current_directory = dir;
       print_string("directory is : ");
	print_hex((u32_t)dir);
	print_string(" ");
	print_hex((u32_t)&dir->tables_physical);
	print_string("\n");
    asm volatile("mov %0,%%cr3"::"r"(&dir->tables_physical));
    u32_t cr0;
    asm volatile("mov %%cr0, %0":"=r"(cr0));
    cr0 |= 0x80000000;
    print_string("before enable paging, cr0 is : ");
    print_hex(cr0);
    print_string(" cr \n");
    asm volatile("mov %0, %%cr0"::"r"(cr0));
    print_string("after enable paging\n");
}

page_t* get_page(u32_t address, int make, page_directory_t *dir) {
    address /= 0x1000;
    u32_t table_index = address/1024;
    if(dir->page_tables[table_index]) {
	return &dir->page_tables[table_index]->pages[address%1024];
    } else if(make) {
	u32_t tmp;
	dir->page_tables[table_index] = (page_table_t*)kmalloc_ap(sizeof(page_table_t), &tmp);
	mem_set((u8_t *)dir->page_tables[table_index],0,0x1000);
	dir->tables_physical[table_index] = tmp|0x7;
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

}
