#include "../common/utils.h"
#include "../boot/gdt.h"
#include "../drivers/interrupt.h"

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
    print_string("Welcome to my os!\n");
    return 1;

}
