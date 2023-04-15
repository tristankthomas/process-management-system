/*
 * min_heap.c - Contains the min heap data structure and functions for the ready queue in SJF
 * Author: Tristan Thomas
 * Date: 8-4-2023
 */

#include <stdio.h>
#include <stdlib.h>
#include <assert.h>

#include "min_heap.h"
#include "process_data.h"

/* Definition of min heap data structure */
struct min_heap {
    process_t **processes;
    int num_items;
    int capacity;
};

static void up_heap(min_heap_t *heap, int index);
static void down_heap(min_heap_t *heap, int index);
static void swap(process_t **p1, process_t **p2);


/**
 * This function creates an empty min heap of size 2
 *
 * @return An empty heap
 */
min_heap_t *create_heap() {

    min_heap_t *heap = malloc(sizeof(*heap));
    assert(heap);
    // allocates memory for processes in heap
    heap->processes = malloc(INIT_SIZE * sizeof(*heap->processes));
    assert(heap->processes);
    heap->num_items = 0;
    heap->capacity = INIT_SIZE;

    return heap;

}

/**
 * This function adds a process to the min heap and heapifies it
 *
 * @param heap Heap that process will be added to
 * @param process Process to add to heap
 */
void insert_process(min_heap_t *heap, process_t *process) {

    if (heap->num_items >= heap->capacity) {
        heap->capacity *= 2;
        heap->processes = realloc(heap->processes, heap->capacity * sizeof(process));
        assert(heap->processes);

    }
    heap->processes[heap->num_items++] = process;

    // fixes heap
    up_heap(heap, heap->num_items - 1);


}

/**
 * This function extracts minimum process from heap and then fixes heap
 *
 * @param heap Heap to be extracted from
 * @return Extracted process
 */
process_t *extract_min(min_heap_t *heap) {

    if (heap->num_items == 0) {
        return NULL;
    }
    // extracts min
    process_t *min = heap->processes[0];
    heap->num_items--;

    // empty heap
    if (heap->num_items == 0) {
        heap->processes[0] = NULL;
        return min;
    }
    // move last element to first
    heap->processes[0] = heap->processes[heap->num_items];

    // fix heap
    down_heap(heap, 0);

    return min;


}

/**
 * Performs up heap operation after process is inserted
 *
 * @param heap Heap to be fixed
 * @param index Index of inserted item
 */
static void up_heap(min_heap_t *heap, int index) {
    int parent = (index - 1) / 2;
    if (index > 0 && cmp_service_time(heap->processes[parent], heap->processes[index]) > 0) {
        process_t *temp = heap->processes[index];
        heap->processes[index] = heap->processes[parent];
        heap->processes[parent] = temp;
        up_heap(heap, parent);
    }
}

/**
 * Performs down heap operation after min is deleted
 *
 * @param heap Heap to be fixed
 * @param index Index of deleted process (0)
 */
 static void down_heap(min_heap_t *heap, int index) {

    int smallest = index;
    int left = 2 * index + 1;
    int right = 2 * index + 2;

    if (left < heap->num_items && cmp_service_time(heap->processes[smallest], heap->processes[left]) > 0) {
        smallest = left;
    }

    if (right < heap->num_items && cmp_service_time(heap->processes[smallest], heap->processes[right]) > 0) {
        smallest = right;
    }

    if (smallest != index) {
        swap(&heap->processes[index], &heap->processes[smallest]);
        down_heap(heap, smallest);
    }

}

/**
 * Swaps two processes in the heap
 *
 * @param p1 Process 1
 * @param p2 Process 2
 */
static void swap(process_t **p1, process_t **p2) {

    process_t *temp = *p1;
    *p1 = *p2;
    *p2 = temp;
}

/**
 * Frees the min heap
 *
 * @param heap Heap to be freed
 */
void free_heap(min_heap_t *heap) {

    free(heap->processes);
    heap->processes = NULL;
    free(heap);
    heap = NULL;

}

/**
 * Gets the heap size
 *
 * @param heap Specified heap
 * @return Number of processes in the heap
 */
int get_heap_size(min_heap_t *heap) {

    return heap->num_items;
}

/**
 * Checks if heap is empty
 *
 * @param heap Heap to be checked
 * @return 1 if empty, 0 otherwise
 */
int is_empty_heap(min_heap_t *heap) {

    return (heap->num_items == 0);
}
