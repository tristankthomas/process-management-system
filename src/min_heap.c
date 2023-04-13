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


struct min_heap {
    process_t **processes;
    int num_items;
    int capacity;
};

min_heap_t *create_heap() {
    min_heap_t *heap;

    heap = (min_heap_t *) malloc(sizeof(*heap));
    heap->processes = (process_t **) malloc(INIT_SIZE * sizeof(*heap->processes));
    assert(heap->processes);
    heap->num_items = 0;
    heap->capacity = INIT_SIZE;

    return heap;

}

void insert_process(min_heap_t *heap, process_t *process) {
    if (heap->num_items >= heap->capacity) {
        heap->capacity *= 2;
        heap->processes = (process_t **) realloc(heap->processes, heap->capacity * sizeof(process));
        assert(heap->processes);

    }
    heap->processes[heap->num_items++] = process;

    up_heap(heap, heap->num_items - 1);


}

process_t *extract_min(min_heap_t *heap) {
    if (heap->num_items == 0) {
        return NULL;
    }
    // extract min
    process_t *min = heap->processes[0];
    heap->num_items--;

    if (heap->num_items == 0) {
        heap->processes[0] = NULL;
        return min;
    }
    // move last element to first
    heap->processes[0] = heap->processes[heap->num_items];


    down_heap(heap, 0);

    return min;


}

void up_heap(min_heap_t *heap, int index) {
    int parent = (index - 1) / 2;
    if (index > 0 && cmp_service_time(heap->processes[parent], heap->processes[index]) > 0) {
        process_t *temp = heap->processes[index];
        heap->processes[index] = heap->processes[parent];
        heap->processes[parent] = temp;
        up_heap(heap, parent);
    }
}

void down_heap(min_heap_t *heap, int index) {
    int smallest = index;
    int left = 2 * index + 1;
    int right = 2 * index + 2;

    if (left < heap->num_items && cmp_service_time(heap->processes[smallest], heap->processes[left]) > 0) {//heap->data[left] < heap->data[smallest]) {
        smallest = left;
    }

    if (right < heap->num_items && cmp_service_time(heap->processes[smallest], heap->processes[right]) > 0) {
        smallest = right;
    }

    if (smallest != index) {
        swap(&heap->processes[index], &heap->processes[smallest]);
        down_heap(heap, smallest);
    }

    // returns if the index does not need to change (smaller than left and right children)
}

void swap(process_t **p1, process_t **p2) {
    process_t *temp = *p1;
    *p1 = *p2;
    *p2 = temp;
}


void free_heap(min_heap_t *heap) {
    free(heap->processes);
    heap->processes = NULL;
    free(heap);
    heap = NULL;
}

int get_heap_size(min_heap_t *heap) {
    return heap->num_items;
}

int is_empty_heap(min_heap_t *heap) {
    return (heap->num_items == 0);
}


