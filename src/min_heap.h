/*
 * min_heap.h - Contains the min heap data structure and functions for the ready queue in SJF
 * Author: Tristan Thomas
 * Date: 8-4-2023
 */

#ifndef MIN_HEAP_H
#define MIN_HEAP_H

#include "process_data.h"

#define INIT_SIZE 2

typedef struct min_heap min_heap_t;
typedef struct process process_t;

min_heap_t *create_heap();
void insert_process(min_heap_t *heap, process_t *process);
process_t *extract_min(min_heap_t *heap);
void up_heap(min_heap_t *heap, int index);
void down_heap(min_heap_t *heap, int index);
void swap(process_t **p1, process_t **p2);
void free_heap(min_heap_t *heap);
int get_heap_size(min_heap_t *heap);
int is_empty_heap(min_heap_t *heap);

#endif
