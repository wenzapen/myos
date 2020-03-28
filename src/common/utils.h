#ifndef UTILS_H
#define UTILS_H

#include "types.h"

void print_string(char *str);
void print_decimal(s32_t n);
void print_hex(s32_t n);

void panic(const char* message, const char* file, u32_t line);
void panic_assert(const char* file, u32_t line, const char* desc);

#define PANIC(msg) panic(msg, __FILE__, __LINE__);
#define ASSERT(b) ((b)?0:panic_assert(__FILE__,__LINE__,#b));

#endif
