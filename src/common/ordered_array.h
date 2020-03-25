#ifndef ORDERED_ARRAY_H
#define ORDERED_ARRAY_H

#include "types.h:


typedef void* type_t;
typedef s8_t (*less_than_t)(type_t, type_t);

typedef struct {
    type_t *array;
    u32_t size;
    u32_t max_size;
    less_than_t less_than;
} ordered_array_t;

ordered_array_t create_ordered_array(u32_t max_size, less_than_t less_than);
ordered_array_t place_ordered_array(void *addr,u32_t max_size, less_than_t less_than);
void insert_ordered_array(type_t item, ordered_array_t *array);
type_t lookup_ordered_array(u32_t i, ordered_array_t *array);
void remove_ordered_array(u32_t i, ordered_array_t *array);
s8_t less_than(type_t, type_t);

#endif
