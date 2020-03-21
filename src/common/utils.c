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
    while(n/10 != 0) {
	str[i] = n%10;
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
