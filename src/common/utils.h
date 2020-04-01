#ifndef UTILS_H
#define UTILS_H

#include "types.h"

void print_char(char character);
void print_string(const char *str);
void print_decimal(s32_t n);
void print_hex(u32_t n);

void panic(const char* message, const char* file, u32_t line);
void panic_assert(const char* file, u32_t line, const char* desc);

void strcpy(char *dest, const char *src);
u32_t strcmp(const char *str1, const char *str2);
u32_t strlen(const char *dest);
void memcpy(char *dest, char *src, u32_t len);

#define PANIC(msg) panic(msg, __FILE__, __LINE__);
#define ASSERT(b) ((b)?0:panic_assert(__FILE__,__LINE__,#b));

#endif
