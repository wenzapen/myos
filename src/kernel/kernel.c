#include "../common/utils.h"
#include "kheap.h"
#include "../boot/gdt.h"
#include "../drivers/interrupt.h"
#include "../drivers/timer.h"
#include "../drivers/keyboard.h"
#include "multiboot.h"
#include "paging.h"
#include "fs.h"
#include "initrd.h"
#include "task.h"

extern u32_t placement_address;
u32_t initial_esp;

int main(multiboot_info_t *multiboot_info, u32_t initial_stack) {
    initial_esp = initial_stack;
    init_serial();
    init_gdt();
    print_string("Welcome to my os!\n");
    init_idt();
    init_timer(50);
    init_keyboard();
    print_string("Welcome to my os!\n");

    ASSERT(multiboot_info->mods_count > 0);
    u32_t module_addr = multiboot_info->mods_addr;
    u32_t initrd_location = *((u32_t *)multiboot_info->mods_addr);
    u32_t initrd_end = *(u32_t *)(multiboot_info->mods_addr + 4);
/*    print_string("Module start address is : ");
    print_hex(module_addr);
    print_string("\n");
    print_string("Module location address is : ");
    print_hex(initrd_location);
    print_string("\n");
    print_string("   Module end address is : ");
    print_hex(initrd_end);
    print_string("\n number of modules: ");
    print_decimal(multiboot_info->mods_count);
    print_string("\n number of files in initrd : ");
    print_decimal(*(u32_t *)initrd_location);
    print_string("\n");
*/    initrd_file_header_t *file = (initrd_file_header_t *)(initrd_location+4);
    typedef void (*call_module_t)(void);
    call_module_t start_program = (call_module_t)module_addr;
    placement_address = initrd_end;
//    start_program();
    init_paging();
    init_task();
/*    u32_t b = kmalloc(8);
    print_string("b: ");
    print_hex(b);
    print_string("\n");
    u32_t c = kmalloc(8);
    print_string("c: ");
    print_hex(c);
    print_string("\n");
*/ 
//   asm volatile("int $3");
 //   u32_t *p = (u32_t*)0xA0000000;
//    u32_t do_page_fault = *p;

    fs_node_t *fs_root = init_initrd(initrd_location);
/*    int ret=fork();
    print_string("fork() returned ");
    print_hex(ret);
    print_string(" , and getpid() returned ");
    print_hex(get_pid());
    print_string("\n");
*/
    asm volatile("cli");
//    print_fs(fs_root);
    asm volatile("sti");
    while(1) ;
    return 1;

}
