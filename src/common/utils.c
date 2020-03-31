#include "utils.h"
#include "../drivers/io.h"
#include "../common/types.h"

void print_string(const char* str) {
    u16_t pos = fb_get_cursor();
    for(int i=0; str[i]!='\0';i++) {
	if(str[i]=='\n') {
	    pos = (pos/MAX_COL+1)*MAX_COL;	
	} else {
	    fb_write(str[i], pos);
	    pos++;
	}
	pos = fb_scroll(pos);
    }
    fb_set_cursor(pos);
}

void print_decimal(s32_t n) {
    char str[35];
    int negative = 0;
    if(n < 0) {
    n = -n;
    negative = 1;
    }
    int i=0;
    str[i] = n%10+'0';
    i++;
    n /= 10;
    while(n != 0) {
	str[i] = n%10+'0';
	n /= 10;
	i++;
    }
    char str2[35];
    i--;
    int j = 0;
    if(negative == 1) {
	str2[j] = '-';
	j++;
    }
    while(i>=0) {
	str2[j++] = str[i--];
    } 
    str2[j] = '\0';
    print_string(str2);
}

void print_hex(u32_t n) {
    char str[35];
    int i=0;
    int h=0; 
    if(n==0) {
	str[0]='0';
	str[1]='x';
	str[2]='0';
	print_string(str);
	return;
    }
    while(n != 0) {
	h = n & 0xf;
	if(h < 10) {
	    str[i] = h + '0';
	} else {
	    str[i] = h - 10 + 'A';
	}
	n = (n >> 4); 
	i++;
    }
    char str2[35];
    i--;
    int j = 0;
    str2[0] = '0';
    str2[1] = 'x';
    j += 2;
    while(i>=0) {
	str2[j++] = str[i--];
    } 
    str2[j] = '\0';
    print_string(str2);
}


void panic(const char* message, const char* file, u32_t line) {
    asm volatile("cli");
    print_string("PANIC(");
    print_string(message);
    print_string(") at ");
    print_string(file);
    print_string(":");
    print_decimal(line);
    print_string("\n");
    while(1) ;

}

void panic_assert(const char* file, u32_t line, const char* desc) {
    asm volatile("cli");
    print_string("ASSERTION-FAILED(");
    print_string(desc);
    print_string(") at ");
    print_string(file);
    print_string(":");
    print_decimal(line);
    print_string("\n");
    while(1) ;

}
/*
void strcpy(char *dest, const char *src) {
    if(dest != src) {
	int i=0;
	while(src[i] != '\0') {
	    dest[i++] = src[i++];
	}
	dest[i] = '\0';
    }
}
*/
void strcpy(char *dest, const char *src) {
    do {
	*dest++ = *src++;
    } while(*src != 0);
}
u32_t strcmp(char *dest, char *src) {
    int i=0;
    while(src[i] != '\0') {
	if(dest[i] != src[i])
	    return 0;
	i++;
    }
    if(dest[i] == '\0')
	return 1;
    return 0;
}
u32_t strlen(char *dest) {
    int i=0;
    while(dest[i] != '\0')
	i++;
    return i;
}
void memcpy(char *dest, char *src, u32_t len) {
    for(int i=0; i<len; i++)
	*dest++ = *src++;
}
