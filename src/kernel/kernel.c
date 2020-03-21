#include "../common/utils.h"
#include "../boot/gdt.h"
#include "../drivers/interrupt.h"
#include "../drivers/timer.h"

int main() {
/*    for(int i=0; i < 100; i++)
	print_string("Hello, My First OS!\n");
    print_string("wow\n");
    print_string("wow\n");
    print_string("wow\n");
    print_string("wow\n");
    print_string("wow\n");
*/
    init_gdt();
    init_idt();
    init_timer(50);
    print_string("Welcome to my os!\n");
    print_decimal(1);
    print_string("\n");
    print_decimal(-1);
    print_string("\n");
    print_decimal(32);
    print_string("\n");
    asm volatile("int $3");
    return 1;

}
