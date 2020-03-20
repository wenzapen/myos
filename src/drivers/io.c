#include "io.h"

#define FB_ADDR 0xb8000
#define FB_COMMAND_PORT 0x3D4
#define FB_DATA_PORT 0x3D5
#define FB_HIGH_BYTE 14
#define FB_LOW_BYTE 15
#define WHITE_ON_BLACK 0x0F

u16_t fb_get_cursor() {
    u16_t pos;
    outb(FB_COMMAND_PORT, FB_HIGH_BYTE);
    pos = inb(FB_DATA_PORT) << 8;
    outb(FB_COMMAND_PORT, FB_LOW_BYTE);
    pos += inb(FB_DATA_PORT);
    return pos;
}

void fb_set_cursor(u16_t position) {
    outb(FB_COMMAND_PORT, FB_HIGH_BYTE);
    outb(FB_DATA_PORT, position >> 8);
    outb(FB_COMMAND_PORT, FB_LOW_BYTE);
    outb(FB_DATA_PORT, position & 0xFF);

}

void fb_write(char character, u16_t position)  {
    u16_t *fb = (u32_t *)FB_ADDR;
    fb[position] = (WHITE_ON_BLACK << 8) | character;

}

u16_t fb_scroll(u16_t position) {
    u8_t *fb = (u32_t *)FB_ADDR;
    if(position < MAX_ROW * MAX_COL) return position;
    for(int i=0; i < MAX_ROW; i++) {
	mem_copy(fb[2*MAX_COL*(i+1)], fb[2*MAX_COL*i], MAX_COL*2);	
    }
    return MAX_COL * (MAX_ROW - 1); 
}

void mem_copy(char* src, char* dest, u32_t len) {
    while(len > 0) {
	*dest++ = *src++;
	len--;
    }
}

void mem_set(char* dest, char character, u32_t len) {
    while(len > 0) {
	*dest++ = character;
	len--;
    } 
}

void outb(u16_t port, u8_t data) {
    asm volatile("out %%al, %%dx" : : "a"(data), "d"(port));

}

u8_t inb(u16_t port) {
    u8_t data;
    asm volatile("in %%dx, %%al" : "=a"(data) : "d"(port));
    return data;
}


