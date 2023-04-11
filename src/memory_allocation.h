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

#define MEM_CAPACITY 2048

typedef enum state state_t;
typedef struct block block_t;
typedef enum block_type block_type_t;



void *allocate_memory(list_t *memory, list_t *holes, list_t *input, void *ready, char *mem_strategy, int sim_time, int (*insert)(void *, process_t *));
void deallocate_memory(process_t *process, char *mem_strategy);
void initialise_memory(list_t *memory, list_t *holes);
block_t *create_block(block_type_t type, int start, int size);
int best_fit(list_t *holes, list_t *memory, process_t *process);
void split(process_t *process, list_node_t *node, list_t *holes, list_t *memory);
int *get_size(block_t *block);
int compare_sizes(int *size1, int *size2);
void process_ready(process_t *process, void *ready, int sim_time, int (*insert)(void *, process_t *));

#endif
