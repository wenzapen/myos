#ifndef TASK_H
#define TASK_H

#include "paging.h"
#include "../common/types.h"

typedef struct task {
    int id;
    u32_t esp, ebp;
    u32_t eip;
    page_directory_t *page_directory;
    struct task *next;
} task_t;

void init_task();
void switch_task();
int fork();
void move_stack(void *new_stack_start, u32_t size);
int get_pid();

#endif

