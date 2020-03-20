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
