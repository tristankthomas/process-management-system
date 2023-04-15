/*
 * min_heap.h - Contains the interface for min heap data structure and functions
 * Author: Tristan Thomas
 * Date: 8-4-2023
 */

#ifndef MIN_HEAP_H
#define MIN_HEAP_H

#include "process_data.h"

#define INIT_SIZE 2

typedef struct min_heap min_heap_t;
typedef struct process process_t;

/**
 * This function creates an empty min heap of size 2
 *
 * @return An empty heap
 */
min_heap_t *create_heap();

/**
 * This function adds a process to the min heap and heapifies it
 *
 * @param heap Heap that process will be added to
 * @param process Process to add to heap
 */
void insert_process(min_heap_t *heap, process_t *process);

/**
 * This function extracts minimum process from heap and then fixes heap
 *
 * @param heap Heap to be extracted from
 * @return Extracted process
 */
process_t *extract_min(min_heap_t *heap);

/**
 * Frees the min heap
 *
 * @param heap Heap to be freed
 */
void free_heap(min_heap_t *heap);

/**
 * Gets the heap size
 *
 * @param heap Specified heap
 * @return Number of processes in the heap
 */
int get_heap_size(min_heap_t *heap);

/**
 * Checks if heap is empty
 *
 * @param heap Heap to be checked
 * @return 1 if empty, 0 otherwise
 */
int is_empty_heap(min_heap_t *heap);

#endif
