/*
 * memory_allocation.c - Contains the functions for the memory allocation algorithms
 * Author: Tristan Thomas
 * Date: 6-4-2023
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>

#include "memory_allocation.h"
#include "linked_list.h"
#include "min_heap.h"

enum state {
    IDLE, READY, RUNNING, FINISHED
};

// make one function
queue_t *allocate_memory_queue(queue_t *input, queue_t *ready, char *mem_strategy) {
    process_t *ready_process;
    if (strcmp(mem_strategy, "infinite") == 0) {

        while (get_head(input) != NULL) {
            ready_process = dequeue(input);
            set_state(ready_process, READY);
            enqueue(ready, ready_process);
        }

    } else if (strcmp(mem_strategy, "best-fit") == 0) {

    }


    return ready;

}

min_heap_t *allocate_memory_pqueue(queue_t *input, min_heap_t *ready, char *mem_strategy) {
    process_t *ready_process;
    if (strcmp(mem_strategy, "infinite") == 0) {

        while (get_head(input) != NULL) {
            ready_process = dequeue(input);
            set_state(ready_process, READY);
            insert(ready, ready_process);
        }

    } else if (strcmp(mem_strategy, "best-fit") == 0) {

    }


    return ready;

}

// make one function







//void *allocate_memory(queue_t *input, void *ready, char *mem_strategy, char *scheduler, int (*insert)(void *, process_t *), void *(*create)(), process_t *process) {
//
//    ready = create();
//    if (strcmp(mem_strategy, "infinite") == 0) {
//        insert(ready, process);
//
//    } else if (strcmp(mem_strategy, "best-fit") == 0) {
//
//    }
//
//
//    return ready;
//
//}