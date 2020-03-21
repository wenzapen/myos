#include "keyboard.h"
#include "../common/utils.h"
#include "interrupt.h"
#include "io.h"

static int kb_self_test();
static void kb_enable();
static void kb_callback(registers_t regs);


void init_keyboard() {
    register_interrupt_handler(33, &kb_callback);

}

static void kb_callback(registers_t regs) {
    print_string("Interrupt : ");
    print_decimal(regs.int_no);
    print_string("\n");
}

static void kb_enable() {


}
