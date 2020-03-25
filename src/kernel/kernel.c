#include "../common/utils.h"
#include "../boot/gdt.h"
#include "../drivers/interrupt.h"
#include "../drivers/timer.h"
#include "../drivers/keyboard.h"
#include "multiboot.h"
#include "paging.h"

int main(multiboot_info_t *multiboot_info) {

    init_gdt();
    print_string("Welcome to my os!\n");
    init_idt();
    init_timer(50);
    init_keyboard();
    print_string("Welcome to my os!\n");

    u32_t module_addr = multiboot_info->mods_addr;
    print_string("Module address is : ");
    print_hex(module_addr);
    print_string("\n");
    print_decimal(multiboot_info->mods_count);
    typedef void (*call_module_t)(void);
    call_module_t start_program = (call_module_t)module_addr;
    
//    start_program();
    init_paging();

    print_decimal(1);
    print_string("\n");
    print_decimal(-1);
    print_string("\n");
    print_decimal(32);
    print_string("\n");
    asm volatile("int $3");
    u32_t *p = (u32_t*)0xA0000000;
    u32_t do_page_fault = *p;
    while(1) ;
    return 1;

}
