#include "ordered_array.h"


s8_t less_than(type_t a, type_t b) {
    return (a < b) ? 1 : 0;
}

ordered_array_t create_ordered_array(u32_t max_size, less_than_t less_than) {
    ordered_array_t to_ret;
    to_ret.array = (type_t *)kmalloc(max_size*sizeof(type_t));
    mem_set((u8_t *)to_ret.array, 0, max_size*sizeof(type_t));
    to_ret.size = 0;
    to_ret.max_size = max_size;
    to_ret.less_than = less_than;
    return to_ret;    

}

ordered_array_t place_ordered_array(void *addr,u32_t max_size, less_than_t less_than) {
    ordered_array_t to_ret;
    to_ret.array = (type_t *)addr;
    print_string("array.array: ");
    print_hex((u32_t)to_ret.array);
    print_string("\n");
    mem_set((u8_t *)to_ret.array, 0, max_size*sizeof(type_t));
    to_ret.size = 0;
    to_ret.max_size = max_size;
    to_ret.less_than = less_than;
    return to_ret;    

}

void insert_ordered_array(type_t item, ordered_array_t *array) {
    ASSERT(array->size<=array->max_size);
    if(array->size == array->max_size) return;
    int i = 0;
    while(i < array->size && array->less_than(array->array[i], item)) 
	i++;
    int j = array->size;
    while(j>i) {
	array->array[j] = array->array[j-1];
	j--;
    }
    array->array[i] = item;
    array->size++;
}

type_t lookup_ordered_array(u32_t i, ordered_array_t *array) {
    ASSERT(i < array->size);
    return array->array[i];

}

void remove_ordered_array(u32_t i, ordered_array_t *array) {
    while(i < array->size-1) {
	array->array[i] = array->array[i+1];
	i++;
    }
    array->size--;
}
