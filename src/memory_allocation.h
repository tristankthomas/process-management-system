/*
 * memory_allocation.h - Contains function declarations for the memory allocation algorithms
 * Author: Tristan Thomas
 * Date: 6-4-2023
 */

#ifndef MEMORY_ALLOCATION_H
#define MEMORY_ALLOCATION_H

#include "linked_list.h"
#include "min_heap.h"
#include "process_data.h"

typedef enum state state_t;

void *allocate_memory(queue_t *input, void *ready, char *mem_strategy, int (*insert)(void *, process_t *));

#endif
