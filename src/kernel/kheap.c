#include "kheap.h"
#include "../common/utils.h"
#include "paging.h"

extern u32_t kernel_end;
u32_t placement_address = (u32_t)&kernel_end;
extern page_directory_t *kernel_directory;
heap_t *kheap=0;

static void print_heap(heap_t *heap) {
    print_string("Heap start address: ");
    print_hex(heap->start_address);
    print_string(" end address: ");
    print_hex(heap->end_address);
    print_string("\n");
    int i = 0;
    while(i < heap->index.size) {
	header_t *h = (header_t *)heap->index.array[i];
	print_string("Hole i: ");
	print_hex(i);
	print_string(" start address: ");
	print_hex((u32_t)h);
	print_string(" Magic: ");
	print_hex(h->magic);
	print_string(" is_hole: ");
	print_hex(h->is_hole);
	print_string(" size: ");
	print_hex(h->size);
	print_string("\n");
	i++;
    }
}


static void expand(u32_t new_size, heap_t *heap) {
    ASSERT(new_size > heap->end_address - heap->start_address);
//make size page-aligned
    if(new_size & 0x00000FFF) {
	new_size &= 0xFFFFF000;
	new_size += 0x1000;
    }
    ASSERT(heap->start_address + new_size <= heap->max_address);
//add page entry and map page to frame
    u32_t old_size = heap->end_address - heap->start_address;
    u32_t i = old_size;
    while(i < new_size) {
	alloc_frame(get_page(heap->start_address+i, 1, kernel_directory),(heap->supervisor)?1:0,(heap->readonly)?0:1);
	i += 0x1000;
    }
    heap->end_address = heap->start_address+new_size;
}


static u32_t kmalloc_internal(u32_t size, int align, u32_t* phys) {
    if(kheap != 0) {
	void *addr = alloc(size, (u8_t)align, kheap);
	print_serial_string("alloc on heap! virtual addr is: ");
	print_serial_hex((u32_t)addr);
	print_serial_string("\n");
	if(phys != 0) {
	    print_serial_string("kernel_dir addr is: ");
	    print_serial_hex((u32_t)kernel_directory);
	    print_serial_string("\n");
//	    print_serial_string("#####################################\n");
//	    print_serial_tables(kernel_directory);
	    page_t *page = get_page((u32_t)addr, 0, kernel_directory);
	    print_serial_string(" Page content for address: ");
	    print_serial_hex((u32_t)addr);
	    print_serial_string(" is : ");
	    print_serial_hex(*(u32_t *)page);
	    print_serial_string("\n");
	    print_serial_string("Page frame for address: ");
	    print_serial_hex((u32_t)addr);
	    print_serial_string(" is : ");
	    print_serial_hex(page->frame);
	    print_serial_string("\n");
	    print_serial_string("Page addr&0xFFF for address: ");
	    print_serial_hex((u32_t)addr);
	    print_serial_string(" is : ");
	    print_serial_hex((u32_t)addr & 0xFFF);
	    print_serial_string("\n");

//	    print_serial_string("page####################################\n");
//	    print_serial_pages(kernel_directory,(u32_t)addr/0x400000);
	    *phys = page->frame*0x1000+ ((u32_t)addr & 0xFFF);
//	    *phys = get_physical_address((u32_t)addr, kernel_directory);
	    print_serial_string("physical addr is: ");
	    print_serial_hex(*phys);
	    print_serial_string("\n");
	}
	return (u32_t)addr;
    } else {

        if(align == 1 && (placement_address & 0xFFF)) {
    	placement_address &= 0xFFFFF000;
    	placement_address += 0x1000;
        }
        if(phys) {
    	*phys = placement_address;
        }
        u32_t tmp = placement_address;
        placement_address += size;
/*    print_string("after Placement_address is : ");
    print_hex(placement_address);
    print_string("\n");
*/        return tmp;
    }

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

static s32_t find_smallest_hole(u32_t size, u8_t page_align, heap_t *heap) {
    int i=0;
/*    print_string("heap->index.size is : ");
    print_hex(heap->index.size);
    print_string("   page_align: ");
    print_hex(page_align);
    print_char('\n');
*/    while(i < heap->index.size) {
//	print_string("Inside find smallest hole: before lookup_order \n");
	header_t *header = (header_t *)lookup_ordered_array(i, &heap->index);
	if(page_align > 0) {
	    u32_t location = (u32_t)header;
/*		print_string("the hole i: ");
		print_hex(i);
		print_string("location: ");
		print_hex(location);
		print_string(" sizeof(header): ");
		print_hex(sizeof(header_t));
		print_char('\n');
		print_hex(sizeof(header_t));
*/
	    int offset = 0;
	    if(((location+sizeof(header_t)) & 0x00000FFF) != 0) {
		offset = 0x1000 - ((location+sizeof(header_t)) & 0x00000FFF);
		s32_t hole_size = (s32_t)header->size - offset;
/*		print_string("after alignment, hole_size: ");
		print_decimal(hole_size);
		print_string(" requried size: ");
		print_decimal(size);
		print_char('\n');
*/		if(hole_size >= (s32_t)size)
		    break;	
	    } 

	} else {
	    if(header->size >= size) 
		break;
	} 
    i++;
/*    print_string("hole number i: ");
    print_decimal(i);
    print_string("\n");
*/	
    }
    if(i >= heap->index.size) {
	return -1;
    }
    return i;
}

static s8_t header_t_less_than(void *a, void *b) {
    return ((header_t *)a)->size < ((header_t *)b)->size ? 1 : 0; 
}
heap_t *create_heap(u32_t start, u32_t end, u32_t max, u8_t supervisor, u8_t readonly) {
    heap_t *heap = (heap_t *)kmalloc(sizeof(heap_t));
    heap->index = place_ordered_array((void *)start, HEAP_INDEX_SIZE, &header_t_less_than);
    start += sizeof(type_t) * HEAP_INDEX_SIZE;
    if(start & 0x00000FFF != 0) {
	start &= 0xFFFFF000;
	start += 0x1000;
    }
    heap->start_address = start;
    heap->end_address = end;
    heap->max_address = max;
    heap->supervisor = supervisor;
    heap->readonly = readonly;

    header_t *hole = (header_t *)start;
    hole->size = end - start;
    hole->magic = HEAP_MAGIC;
    hole->is_hole = 1;
    insert_ordered_array((void *)hole, &heap->index);
    return heap;

}

void *alloc(u32_t size, u8_t page_align, heap_t *heap) {
    //print_heap(heap);
    u32_t new_size = size + sizeof(header_t) + sizeof(footer_t);
//    print_string("before finding smallest hole: ");
    s32_t i = find_smallest_hole(new_size, page_align, heap);
/*    print_string("page_align: ");
    print_decimal(page_align);
    print_string("  hole: ");
    print_decimal(i);
*/    print_char('\n');
    if(i == -1) {  //if no fit resource available, expand the heap
	u32_t old_length = heap->end_address - heap->start_address;
	u32_t old_end_address = heap->end_address;
	expand(old_length + new_size, heap);
	u32_t new_length = heap->end_address - heap->start_address;
	i = 0;
	u32_t idx = -1; u32_t value = 0x0;
	while(i < heap->index.size) {
	    u32_t tmp = (u32_t)lookup_ordered_array(i, &heap->index);
	    if(tmp > value) {
		value = tmp;
		idx = i;
	    }
	    i++;
	}
	if(idx == -1) {
	    header_t *header = (header_t *)old_end_address;
	    header->magic = HEAP_MAGIC;
	    header->size = new_length - old_length;
	    header->is_hole = 1;
	    footer_t *footer = (footer_t *)(old_end_address+header->size - sizeof(footer_t));
	    footer->magic = HEAP_MAGIC;
	    footer->header = header;
	    insert_ordered_array((void *)header, &heap->index);
	} else {
	    header_t *header = lookup_ordered_array(idx, &heap->index);
	    header->size += new_length - old_length;
	    footer_t *footer = (footer_t *)((u32_t)header+header->size-sizeof(footer_t));
	    footer->header = header;
	    footer->magic = HEAP_MAGIC;
	}

	return alloc(size, page_align, heap);
    } //resource found on heap
    header_t *orig_hole_header = (header_t *)lookup_ordered_array(i, &heap->index);
    u32_t orig_hole_pos = (u32_t)orig_hole_header;
    u32_t orig_hole_size = orig_hole_header->size;
    if(orig_hole_size-new_size < sizeof(header_t)+sizeof(footer_t)) {
	size += orig_hole_size - new_size;
	new_size = orig_hole_size;
    }
/*    print_string("  orig_hole_pos: ");
    print_hex(orig_hole_pos);
*/    print_char('\n');
    if(page_align && ((orig_hole_pos+sizeof(header_t)) & 0x00000FFF)) {
//	print_string("page_align is required.\n");
	u32_t new_location = orig_hole_pos + 0x1000 - (orig_hole_pos&0xFFF) - sizeof(header_t);
	header_t *hole_header = (header_t *)orig_hole_pos;
	hole_header->size = 0x1000-(orig_hole_pos&0xFFF)-sizeof(header_t);
	hole_header->magic = HEAP_MAGIC;
	hole_header->is_hole = 1;
	footer_t *hole_footer = (footer_t *)((u32_t)new_location - sizeof(footer_t));
	hole_footer->magic = HEAP_MAGIC;
	hole_footer->header = hole_header;
	orig_hole_pos = new_location;
	orig_hole_size = orig_hole_size - hole_header->size;
    } else {
	remove_ordered_array(i, &heap->index);
    }
    header_t *block_header = (header_t *)orig_hole_pos;
    block_header->magic = HEAP_MAGIC;
    block_header->is_hole = 0;
    block_header->size = new_size;
    footer_t *block_footer = (footer_t *)(orig_hole_pos+sizeof(header_t)+size);
    block_footer->magic = HEAP_MAGIC;
    block_footer->header = block_header;
    if(orig_hole_size - new_size > 0) {
	header_t *hole_header=(header_t *)(orig_hole_pos+sizeof(header_t)+size+sizeof(footer_t));
	hole_header->magic = HEAP_MAGIC;
	hole_header->is_hole = 1;
	hole_header->size = orig_hole_size - new_size;
	footer_t *hole_footer = (footer_t *)((u32_t)hole_header+orig_hole_size-new_size-sizeof(footer_t));
	if((u32_t)hole_footer < heap->end_address) {
	    hole_footer->magic = HEAP_MAGIC;
	    hole_footer->header = hole_header;
	}
	insert_ordered_array((void*)hole_header, &heap->index);
    }
    return (void *)((u32_t)block_header+sizeof(header_t));
}
