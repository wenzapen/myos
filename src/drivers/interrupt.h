#ifndef INTERRUPT_H
#define INTERRUPT_H
#include "../common/types.h"

#define PIC1_PORT_COMMAND 0x20
#define PIC1_PORT_DATA 0x21
#define PIC2_PORT_COMMAND 0xA0
#define PIC2_PORT_DATA 0xA1
#define PIC1_START_INTERRUPT 0x20
#define PIC2_START_INTERRUPT 0x28
#define PIC2_END_INTERRUPT PIC2_START_INTERRUPT + 7
#define PIC_ACK 0x20

struct idt_struct {
    u16_t limit;
    u32_t base;
} __attribute__((packed));
typedef struct idt_struct idt_t;

struct idt_entry_struct {
    u16_t base_low;
    u16_t segment;
    u8_t always0;
    u8_t flags;
    u16_t base_high;
} __attribute__((packed));
typedef struct idt_entry_struct idt_entry_t;

typedef struct registers_struct {
    u32_t ds;
    u32_t edi,esi,ebp,esp,ebx,edx,ecx,eax;
    u32_t int_no,err_code;
    u32_t eip,cs,eflags,useresp,ss;
} registers_t;

typedef void (*interrupt_handler_t)(registers_t);
void register_interrupt_handler(u32_t int_no, interrupt_handler_t handler);
void init_idt();

extern void  isr0(void);
extern void  isr1(void);
extern void  isr2(void);
extern void  isr3(void);
extern void  isr4(void);
extern void  isr5(void);
extern void  isr6(void);
extern void  isr7(void);
extern void  isr8(void);
extern void  isr9(void);
extern void isr10(void);
extern void isr11(void);
extern void isr12(void);
extern void isr13(void);
extern void isr14(void);
extern void isr15(void);
extern void isr16(void);
extern void isr17(void);
extern void isr18(void);
extern void isr19(void);
extern void isr20(void);
extern void isr21(void);
extern void isr22(void);
extern void isr23(void);
extern void isr24(void);
extern void isr25(void);
extern void isr26(void);
extern void isr27(void);
extern void isr28(void);
extern void isr29(void);
extern void isr30(void);
extern void isr31(void);
extern void  irq0(void);
extern void  irq1(void);
extern void  irq2(void);
extern void  irq3(void);
extern void  irq4(void);
extern void  irq5(void);
extern void  irq6(void);
extern void  irq7(void);
extern void  irq8(void);
extern void  irq9(void);
extern void irq10(void);
extern void irq11(void);
extern void irq12(void);
extern void irq13(void);
extern void irq14(void);
extern void irq15(void);

#endif
