#include "interrupt.h"
#include "../common/utils.h"

idt_t idt;
idt_entry_t idt_entries[256];
interrupt_handler_t interrupt_handler_entries[256];

extern void load_idt(u32_t);

static void idt_set_gate(u32_t n,u32_t base, u16_t segment,u8_t flags) {
    idt_entries[n].base_low = base & 0xffff;
    idt_entries[n].base_high = (base >> 16) & 0xffff;
    idt_entries[n].segment = segment;
    idt_entries[n].always0 = 0;
    idt_entries[n].flags = flags;
}

void init_idt() {
    idt.limit = sizeof(idt_entry_t)*256 - 1;
    idt.base = (u32_t)&idt_entries;

    idt_set_gate( 0,  (u32_t)isr0, 0x08, 0x8E);
    idt_set_gate( 1,  (u32_t)isr1, 0x08, 0x8E);
    idt_set_gate( 2,  (u32_t)isr2, 0x08, 0x8E);
    idt_set_gate( 3,  (u32_t)isr3, 0x08, 0x8E);
    idt_set_gate( 4,  (u32_t)isr4, 0x08, 0x8E);
    idt_set_gate( 5,  (u32_t)isr5, 0x08, 0x8E);
    idt_set_gate( 6,  (u32_t)isr6, 0x08, 0x8E);
    idt_set_gate( 7,  (u32_t)isr7, 0x08, 0x8E);
    idt_set_gate( 8,  (u32_t)isr8, 0x08, 0x8E);
    idt_set_gate( 9,  (u32_t)isr9, 0x08, 0x8E);
    idt_set_gate(10, (u32_t)isr10, 0x08, 0x8E);
    idt_set_gate(11, (u32_t)isr11, 0x08, 0x8E);
    idt_set_gate(12, (u32_t)isr12, 0x08, 0x8E);
    idt_set_gate(13, (u32_t)isr13, 0x08, 0x8E);
    idt_set_gate(14, (u32_t)isr14, 0x08, 0x8E);
    idt_set_gate(15, (u32_t)isr15, 0x08, 0x8E);
    idt_set_gate(16, (u32_t)isr16, 0x08, 0x8E);
    idt_set_gate(17, (u32_t)isr17, 0x08, 0x8E);
    idt_set_gate(18, (u32_t)isr18, 0x08, 0x8E);
    idt_set_gate(19, (u32_t)isr19, 0x08, 0x8E);
    idt_set_gate(20, (u32_t)isr20, 0x08, 0x8E);
    idt_set_gate(21, (u32_t)isr21, 0x08, 0x8E);
    idt_set_gate(22, (u32_t)isr22, 0x08, 0x8E);
    idt_set_gate(23, (u32_t)isr23, 0x08, 0x8E);
    idt_set_gate(24, (u32_t)isr24, 0x08, 0x8E);
    idt_set_gate(25, (u32_t)isr25, 0x08, 0x8E);
    idt_set_gate(26, (u32_t)isr26, 0x08, 0x8E);
    idt_set_gate(27, (u32_t)isr27, 0x08, 0x8E);
    idt_set_gate(28, (u32_t)isr28, 0x08, 0x8E);
    idt_set_gate(29, (u32_t)isr29, 0x08, 0x8E);
    idt_set_gate(30, (u32_t)isr30, 0x08, 0x8E);
    idt_set_gate(31, (u32_t)isr31, 0x08, 0x8E);
    idt_set_gate(32,  (u32_t)irq0, 0x08, 0x8E);
    idt_set_gate(33,  (u32_t)irq1, 0x08, 0x8E);
    idt_set_gate(34,  (u32_t)irq2, 0x08, 0x8E);
    idt_set_gate(35,  (u32_t)irq3, 0x08, 0x8E);
    idt_set_gate(36,  (u32_t)irq4, 0x08, 0x8E);
    idt_set_gate(37,  (u32_t)irq5, 0x08, 0x8E);
    idt_set_gate(38,  (u32_t)irq6, 0x08, 0x8E);
    idt_set_gate(39,  (u32_t)irq7, 0x08, 0x8E);
    idt_set_gate(40,  (u32_t)irq8, 0x08, 0x8E);
    idt_set_gate(41,  (u32_t)irq9, 0x08, 0x8E);
    idt_set_gate(43, (u32_t)irq11, 0x08, 0x8e);
    idt_set_gate(44, (u32_t)irq12, 0x08, 0x8e);
    idt_set_gate(45, (u32_t)irq13, 0x08, 0x8e);
    idt_set_gate(46, (u32_t)irq14, 0x08, 0x8E);
    idt_set_gate(47, (u32_t)irq15, 0x08, 0x8E);

    load_idt((u32_t)&idt);

}

void isr_handler(registers_t regs) {
    if(interrupt_handler_entries[regs.int_no] != 0) {
	interrupt_handler_t handler = interrupt_handler_entries[regs.int_no];
	handler(regs);
    } else {
	print_string("Isr : ");
	print_decimal(regs.int_no);
	print_string(" is not handlered\n");
    }
}

void irq_handler(registers_t regs) {
    if(interrupt_handler_entries[regs.int_no] != 0) {
	interrupt_handler_t handler = interrupt_handler_entries[regs.int_no];
	handler(regs);
    } else {
	print_string("Interrupt : ");
	print_decimal(regs.int_no);
	print_string(" is not handlered\n");
    }

}

void register_interrupt_handler(u32_t int_no, interrupt_handler_t handler) {
    interrupt_handler_entries[int_no] = handler;
}
