#include "keyboard.h"
#include "../common/utils.h"
#include "interrupt.h"
#include "io.h"

#define KB_ENC_INPUT_BUF 0x60
#define KB_ENC_CMD_REG 0x60
#define KB_CTRL_STATS_REG 0x64
#define KB_CTRL_CMD_REG 0x64
#define KB_CTRL_STATS_MASK_OUT_BUF 1
#define KB_CTRL_STATS_MASK_IN_BUF 2

static int kb_self_test();
static void kb_enable();
static u8_t kb_ctrl_read_status();
static void kb_ctrl_send_cmd(u8_t cmd);
static void kb_enc_send_cmd(u8_t cmd);
static u8_t kb_enc_read_buf();
static void kb_set_cap_led(u8_t cap);
static void kb_callback(registers_t regs);


void init_keyboard() {
    register_interrupt_handler(33, &kb_callback);
    kb_set_cap_led(1);

}

static void kb_callback(registers_t regs) {
    print_string("Interrupt : ");
    print_decimal(regs.int_no);
    print_string("\n");
    int code;
    if(kb_ctrl_read_status() & KB_CTRL_STATS_MASK_OUT_BUF == 1) {
	code = kb_enc_read_buf();
	print_string("Scancode is : ");
	print_decimal(code);
	print_string("\n");	
    } 
}

static void kb_enable() {
}

static u8_t kb_ctrl_read_status() {
    return inb(KB_CTRL_STATS_REG);
}

static void kb_ctrl_send_cmd(u8_t cmd) {
    while(1) {
	if(kb_ctrl_read_status() & KB_CTRL_STATS_MASK_IN_BUF == 0)
	    break;
    }   
    outb(KB_CTRL_CMD_REG, cmd); 
}

static void kb_enc_send_cmd(u8_t cmd) {
    while(1) {
	if(kb_ctrl_read_status() & KB_CTRL_STATS_MASK_IN_BUF == 0)
	    break;
    }   
    outb(KB_ENC_CMD_REG, cmd); 

}

static u8_t kb_enc_read_buf() {
    return inb(KB_ENC_INPUT_BUF);
}

static void kb_set_cap_led(u8_t cap) {
    kb_enc_send_cmd(0xED);
    kb_enc_send_cmd(cap << 2);
}
