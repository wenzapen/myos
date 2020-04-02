#include "utils.h"
#include "../drivers/io.h"
#include "../common/types.h"

#define SERIAL_COM1_BASE 0x3F8
#define SERIAL_DATA_PORT(base) (base)
#define SERIAL_FIFO_COMMAND_PORT(base) (base+2)
#define SERIAL_LINE_COMMAND_PORT(base) (base+3)
#define SERIAL_MODEM_COMMAND_PORT(base) (base+4)
#define SERIAL_LINE_STATUS_PORT(base) (base+5)

void print_char(char character) {
    u16_t pos = fb_get_cursor();
    if(character == '\n')
	pos = (pos/MAX_COL+1)*MAX_COL;
    else {
	fb_write(character, pos);
	pos++;
    }
    pos = fb_scroll(pos);
    fb_set_cursor(pos);
}

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

void strcpy(char *dest, const char *src) {
    if(dest != src) {
	int i=0;
	while(*src != 0) {
	    *dest++ = *src++;
	}
	*dest = '\0';
    }
}

/*
void strcpy(char *dest, const char *src) {
    do {
	*dest++ = *src++;
    } while(*src != 0);
}
*/

//if str1 < str2, return -1; if str1=str2, return 0;if str1>str2,return 1
u32_t strcmp(const char *str1, const char *str2) {
    const unsigned char *s1 = str1;
    const unsigned char *s2 = str2;
    unsigned char c1, c2;
    do {
	c1 = *s1++;
	c2 = *s2++;
	if(c1 == '\0')
	    return c1 - c2;
    } while(c1 == c2);
    return c1 - c2;
}
u32_t strlen(const char *dest) {
    int i=0;
    while(dest[i] != '\0')
	i++;
    return i;
}
void memcpy(char *dest, const char *src, u32_t len) {
    const char* s = src;
    for(int i=0; i<len; i++)
	*dest++ = *s++;
}

void memset(char* dest, char character, u32_t len) {
    while(len > 0) {
        *(dest++) = character;
        len--;
    }
}

static void serial_configure_baud_rate(u16_t com,u16_t divisor) {
    outb(SERIAL_LINE_COMMAND_PORT(com),0x80);
    outb(SERIAL_DATA_PORT(com),(divisor>>8)&0x00ff);
    outb(SERIAL_DATA_PORT(com),divisor&0x00ff);

}
static void serial_configure_line(u16_t com) {
    outb(SERIAL_LINE_COMMAND_PORT(com), 0x03);
}
static void serial_configure_buffer(u16_t com) {
    outb(SERIAL_FIFO_COMMAND_PORT(com), 0xc7);
}
static void serial_configure_modem(u16_t com) {
    outb(SERIAL_MODEM_COMMAND_PORT(com), 0x03);
}

static int serial_is_transmit_fifo_empty(u16_t com) {
    return inb(SERIAL_LINE_STATUS_PORT(com)) & 0x20;
}

void init_serial() {
    serial_configure_baud_rate(SERIAL_COM1_BASE, 0x3);
    serial_configure_line(SERIAL_COM1_BASE);
    serial_configure_buffer(SERIAL_COM1_BASE);
    serial_configure_modem(SERIAL_COM1_BASE);

}
void write_serial_char(char character) {
    while(!serial_is_transmit_fifo_empty(SERIAL_COM1_BASE)) ;
    outb(SERIAL_DATA_PORT(SERIAL_COM1_BASE), character);

}

void write_serial_string(char *str) {
    char * s = str;
    while(*s != 0) {
	write_serial_char(*s);
	s++;
    }
}


void write_serial_decimal(s32_t n) {
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
    write_serial_string(str2);
}

void write_serial_hex(u32_t n) {
    char str[35];
    int i=0;
    int h=0; 
    if(n==0) {
	str[0]='0';
	str[1]='x';
	str[2]='0';
	write_serial_string(str);
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
    write_serial_string(str2);
}
