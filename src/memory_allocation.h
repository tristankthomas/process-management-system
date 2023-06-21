/*
 * memory_allocation.h - Contains interface for memory allocation
 * Author: Tristan Thomas
 * Date: 6-4-2023
 */

#ifndef MEMORY_ALLOCATION_H
#define MEMORY_ALLOCATION_H

#define MEM_CAPACITY 2048

#include <stdint.h>

typedef enum state state_t;
typedef struct block block_t;
typedef enum block_type block_type_t;
typedef struct list list_t;
typedef struct node node_t;
typedef struct process process_t;
typedef int (*insert_func)(void *, process_t *);


/**
 * Allocates memory to a process depending on the memory strategy
 *
 * @param memory Memory management data structure
 * @param holes List of free memory blocks (nodes of memory)
 * @param input List of unallocated and arrived processes
 * @param ready List of allocated processes (data structure depending on scheduling algorithm)
 * @param mem_strategy Memory allocation strategy
 * @param sim_time Simulation time
 * @param insert Function to insert process into ready queue
 * @return Ready queue
 */
void *allocate_memory(list_t *memory, list_t *holes, list_t *input, void *ready, char *mem_strategy, uint32_t sim_time,
                      insert_func insert);

/**
 * Deallocates the memory for a process once it is terminated
 *
 * @param process Process to be deallocated
 * @param memory Memory management list
 * @param holes List of holes
 * @param mem_strategy Memory strategy
 */
void deallocate_memory(process_t *process, list_t *memory, list_t *holes, char *mem_strategy);

/**
 * Initialises memory and holes to 2048 Bytes
 *
 * @param memory Memory management list
 * @param holes List of holes
 */
void initialise_memory(list_t *memory, list_t *holes);

/**
 * Gets size of block
 *
 * @param block Specified block
 * @return Size of block
 */
int *get_size(block_t *block);

/**
 * Compares two ints - used to compare block sizes when inserting block into holes list
 *
 * @param int1 Pointer to int
 * @param int2 Pointer to int
 * @return Returns 1, 0 or -1 depending on relative int values
 */
int compare_ints(int *int1, int *int2);

#endif
