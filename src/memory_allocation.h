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

queue_t *allocate_memory_queue(queue_t *input, queue_t *ready, char *mem_strategy);
min_heap_t *allocate_memory_pqueue(queue_t *input, min_heap_t *ready, char *mem_strategy);

#endif
