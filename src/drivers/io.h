#ifndef IO_H
#define IO_H

#include "../common/types.h"

#define MAX_ROW 25
#define MAX_COL 80

void outb(u16_t port, u8_t data);
u8_t inb(u16_t port);
void fb_set_cursor(u16_t position);
u16_t fb_get_cursor();
void fb_write(char character, u16_t position);
u16_t fb_scroll(u16_t position);
void mem_copy(char* src, char* dest, u32_t len);
void mem_set(char*dest, char character, u32_t len);


#endif
