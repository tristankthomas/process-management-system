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

void *allocate_memory(queue_t *input, void *ready, char *mem_strategy, int (*insert)(void *, process_t *)) {
    process_t *ready_process;
    if (strcmp(mem_strategy, "infinite") == 0) {

        // transfer the process straight to ready
        while (get_head(input) != NULL) {
            ready_process = dequeue(input);
            set_state(ready_process, READY);
            insert(ready, ready_process);
        }

    } else if (strcmp(mem_strategy, "best-fit") == 0) {




    }

    return ready;
}

void deallocate_memory(process_t *process, char *mem_strategy) {
    if (strcmp(mem_strategy, "infinity") == 0) {


    } else if (strcmp(mem_strategy, "best-fit") == 0) {


    }

}
