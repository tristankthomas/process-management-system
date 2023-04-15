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
    void **data;
    int num_items;
    int capacity;
};

static void up_heap(min_heap_t *heap, int index, int (compare)(void *, void *));
static void down_heap(min_heap_t *heap, int index, int (compare)(void *, void *));
static void swap(void **p1, void **p2);


/**
 * This function creates an empty min heap of size 2
 *
 * @return An empty heap
 */
min_heap_t *create_heap() {

    min_heap_t *heap = malloc(sizeof(*heap));
    assert(heap);
    // allocates memory for data in heap
    heap->data = malloc(INIT_SIZE * sizeof(*heap->data));
    assert(heap->data);
    heap->num_items = 0;
    heap->capacity = INIT_SIZE;

    return heap;

}

/**
 * This function adds a data element to the min heap and heapifies it
 *
 * @param heap Heap that the data element will be added to
 * @param data Data element to add to heap
 */
void insert_data(min_heap_t *heap, void *data) {

    if (heap->num_items >= heap->capacity) {
        heap->capacity *= 2;
        heap->data = realloc(heap->data, heap->capacity * sizeof(data));
        assert(heap->data);

    }
    heap->data[heap->num_items++] = data;

    // fixes heap (need to use specific comparison function here as num_args(insert_data) == num_args(enqueue))
    up_heap(heap, heap->num_items - 1, (int (*)(void *, void *)) cmp_service_time);


}

/**
 * This function extracts minimum data element from heap and then fixes heap
 *
 * @param heap Heap to be extracted from
 * @return Extracted data element
 */
void *extract_min(min_heap_t *heap) {

    if (heap->num_items == 0) {
        return NULL;
    }
    // extracts min
    void *min = heap->data[0];
    heap->num_items--;

    // empty heap
    if (heap->num_items == 0) {
        heap->data[0] = NULL;
        return min;
    }
    // move last element to first
    heap->data[0] = heap->data[heap->num_items];

    // fix heap (need to use specific comparison function here as num_args(extract_min) == num_args(dequeue))
    down_heap(heap, 0, (int (*)(void *, void *)) cmp_service_time);

    return min;


}

/**
 * Performs up heap operation after data element is inserted
 *
 * @param heap Heap to be fixed
 * @param index Index of inserted item
 */
static void up_heap(min_heap_t *heap, int index, int (compare)(void *, void *)) {
    int parent = (index - 1) / 2;
    if (index > 0 && compare(heap->data[parent], heap->data[index]) > 0) {
        void *temp = heap->data[index];
        heap->data[index] = heap->data[parent];
        heap->data[parent] = temp;
        up_heap(heap, parent, compare);
    }
}

/**
 * Performs down heap operation after min is deleted
 *
 * @param heap Heap to be fixed
 * @param index Index of deleted data element (0)
 */
 static void down_heap(min_heap_t *heap, int index, int (compare)(void *, void *)) {

    int smallest = index;
    int left = 2 * index + 1;
    int right = 2 * index + 2;

    if (left < heap->num_items && compare(heap->data[smallest], heap->data[left]) > 0) {
        smallest = left;
    }

    if (right < heap->num_items && cmp_service_time(heap->data[smallest], heap->data[right]) > 0) {
        smallest = right;
    }

    if (smallest != index) {
        swap(&heap->data[index], &heap->data[smallest]);
        down_heap(heap, smallest, compare);
    }

}

/**
 * Swaps two data elements in the heap
 *
 * @param d1 Data 1
 * @param d2 Data 2
 */
static void swap(void **d1, void **d2) {

    void *temp = *d1;
    *d1 = *d2;
    *d2 = temp;
}

/**
 * Frees the min heap
 *
 * @param heap Heap to be freed
 */
void free_heap(min_heap_t *heap) {

    free(heap->data);
    heap->data = NULL;
    free(heap);
    heap = NULL;

}

/**
 * Gets the heap size
 *
 * @param heap Specified heap
 * @return Number of data elements in the heap
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
