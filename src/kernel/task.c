#include "task.h"
#include "paging.h"
#include "kheap.h"
#include "../common/utils.h"

volatile task_t *current_task;
volatile task_t *ready_queue;

extern page_directory_t *kernel_directory;
extern page_directory_t *current_directory;
extern u32_t initial_esp;
extern u32_t read_eip();

u32_t next_pid = 1;

void init_task() {
//    print_string("Initialising tasking!\n");
    asm volatile("cli");
//    print_string("Before moving stack!\n");
    move_stack((void *)0xE0000000, 0x4000); 
//    print_string("After moving stack!\n");
    current_task = ready_queue = (task_t *)kmalloc(sizeof(task_t));
    current_task->id = next_pid++;
    current_task->esp = current_task->ebp = 0;
    current_task->eip = 0;
    current_task->page_directory = current_directory;
    current_task->next = 0;
    asm volatile("sti");


}


void move_stack(void *new_stack_start, u32_t size) {
    u32_t i;
    for(i=(u32_t)new_stack_start; i>=((u32_t)new_stack_start-size); i-=0x1000) {
	alloc_frame(get_page(i,1,current_directory), 0, 1);
    }
//    print_string("Inside move_stack: after alloc_frame\n");
    print_serial_tables(current_directory);
    u32_t pd_addr;
    asm volatile("mov %%cr3, %0":"=r"(pd_addr));
    asm volatile("mov %0, %%cr3": : "r"(pd_addr));
    print_string("page_directory address: ");
    print_hex(pd_addr);
    print_string("\n");

    u32_t old_esp; asm volatile("mov %%esp, %0" : "=r"(old_esp));
    u32_t old_ebp; asm volatile("mov %%ebp, %0" : "=r"(old_ebp));

    u32_t offset = (u32_t)new_stack_start - initial_esp;
    u32_t new_esp = old_esp + offset;
    u32_t new_ebp = old_ebp + offset;

    memcpy((void *)new_esp, (void *)old_esp, initial_esp-old_esp);
    
    for(i=(u32_t)new_stack_start; i >(u32_t)new_stack_start-size; i-=4) {
	u32_t tmp = *(u32_t *)i;
	if((tmp < initial_esp) && (tmp > old_esp)) {
	    tmp = tmp + offset;
	    u32_t *tmp2 = (u32_t *)i;
	    *tmp2 = tmp;
	}
    }
    asm volatile("mov %0, %%esp" : : "r"(new_esp));
    asm volatile("mov %0, %%ebp" : : "r"(new_ebp));
}

void switch_task() {
    if(!current_task) return;
    u32_t esp, ebp, eip;
    asm volatile("mov %%esp, %0" : "=r"(esp));
    asm volatile("mov %%ebp, %0" : "=r"(ebp));

    eip = read_eip();
    if(eip == 0x12345) return;
    current_task->eip = eip;
    current_task->esp = esp;
    current_task->ebp = ebp;
    current_task= current_task->next;
    if(!current_task) current_task = ready_queue;
    esp = current_task->esp;
    ebp = current_task->ebp;
    asm volatile(" \
	cli;	\
	mov %0, %%ecx;	\
	mov %1, %%esp;	\
	mov %2, %%ebp;	\
	mov %3, %%cr3;	\
	mov $0x12345, %%eax;	\
	sti;	\
	jmp *%%ecx" : : "r"(eip),"r"(esp),"r"(ebp),"r"(current_directory->physical_addr));

}

int fork() {
    asm volatile("cli");
    task_t *parent_task = (task_t*)current_task;
    page_directory_t *directory = clone_directory(current_directory);
    task_t *new_task = (task_t *)kmalloc(sizeof(task_t));
    new_task->id = next_pid++;
    new_task->esp = new_task->ebp = 0;
    new_task->eip = 0;
    new_task->page_directory = directory;
    new_task->next = 0;

    task_t *tmp_task = (task_t *)ready_queue;
    while(tmp_task->next) tmp_task = tmp_task->next;
    tmp_task->next = new_task;

    u32_t eip = read_eip();

    if(current_task == parent_task) {
	u32_t esp; asm volatile("mov %%esp, %0" : "=r"(esp));
	u32_t ebp; asm volatile("mov %%ebp, %0" : "=r"(ebp));
        new_task->esp = esp;
	new_task->ebp = ebp;
	new_task->eip = eip;
	asm volatile("sti");
	return new_task->id;
    } else {
	return 0;
    }
}

int get_pid() {
    return current_task->id;
}
