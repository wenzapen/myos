#include "utils.h"
#include "../drivers/io.h"
#include "../common/types.h"

void print_string(char* str) {
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
    char str[32];
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
    char str2[32];
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

void print_hex(s32_t n) {
    char str[32];
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
    char str2[32];
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
