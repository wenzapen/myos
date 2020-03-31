#include "timer.h"
#include "interrupt.h"
#include "../common/utils.h"
#include "io.h"

u32_t tick = 0;

static void timer_callback(registers_t regs) {
    tick++;
/*    if(tick < 5 ) {
        print_string("Tick: ");
        print_decimal(tick);
        print_string("\n");
    }
*/
}

void init_timer(u32_t frequency) {
    register_interrupt_handler(32, &timer_callback);
    u32_t divisor = 1193180/frequency;
    outb(0x43, 0x36);
    u8_t l = (u8_t)(divisor & 0xff);
    u8_t h = (u8_t)((divisor >> 8) & 0xff);
    outb(0x40, l);
    outb(0x40, h);

}

