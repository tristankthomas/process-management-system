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
#include "process_data.h"

enum state {
    IDLE, READY, RUNNING, FINISHED
};

enum block_type {
    PROCESS, HOLE
};

struct block {
    block_type_t type;
    int start_address;
    int size;
};





void *allocate_memory(list_t *memory, list_t *holes, list_t *input, void *ready, char *mem_strategy, int sim_time, int (*insert)(void *, process_t *)) {
    process_t *process;
    if (strcmp(mem_strategy, "infinite") == 0) {

        // transfer the process straight to ready
        while ((process = dequeue(input)) != NULL) {
            process_ready(process, ready, sim_time, mem_strategy, insert);
        }

    } else if (strcmp(mem_strategy, "best-fit") == 0) {

        while ((process = dequeue(input)) != NULL) {

            if (best_fit(holes, memory, process)) {
                process_ready(process, ready, sim_time, mem_strategy, insert);
            }
        }


    }

    return ready;
}


int best_fit(list_t *holes, list_t *memory, process_t *process) {
    int is_space = 1;
    list_node_t *curr = get_head(holes);
    list_node_t *prev = curr;
    list_node_t *best_fit = NULL;

    // try to refactor this!
    while ((is_space = (curr != NULL))) {
        block_t *curr_block = (block_t *) get_data(get_data(curr));
        // if I hit the end and the previous hole is big enough then pick that
        // when the first memory block that is too large is found (previous one will be best fit)
        if (get_value(process, 'm') > curr_block->size) {
            best_fit = prev;
            break;
        } else if (get_next(curr) == NULL && (curr_block->size >= get_value(process, 'm'))) {
            best_fit = curr;
            break;
        }
        prev = curr;
        curr = get_next(curr);
    }

    if (is_space) {
        delete_node(holes, best_fit);
        split(process, get_data(best_fit), holes, memory);
    }

    return is_space;

}


void split(process_t *process, list_node_t *node, list_t *holes, list_t *memory) {

    // memory block from previous node
    block_t *old_block = (block_t *) get_data(node);

    int prev_size = old_block->size;
    int new_size = get_value(process, 'm');
    int start_address = old_block->start_address;
    // block stored in new node
    block_t *new_block = create_block(PROCESS, start_address, new_size);
    set_block(process, new_block);
    // (block_t *) get_data(memory->head)
    old_block->type = HOLE;
    old_block->start_address = start_address + new_size;
    old_block->size = prev_size - new_size;

    insert_list_node(memory, new_block, get_prev(node), node);

    insert_node_sorted(holes, node, (int (*)(void *, void *)) compare_sizes, (void *(*)(void *)) get_size);


}

void deallocate_memory(process_t *process, char *mem_strategy) {
    if (strcmp(mem_strategy, "infinity") == 0) {


    } else if (strcmp(mem_strategy, "best-fit") == 0) {


    }

}


void initialise_memory(list_t *memory, list_t *holes) {
    block_t *init_hole = create_block(HOLE, 0, MEM_CAPACITY);
    enqueue(memory, init_hole);
    enqueue(holes, get_head(memory));
}


block_t *create_block(block_type_t type, int start, int size) {
    block_t *block = (block_t *) malloc(sizeof(*block));
    block->type = type;
    block->size = size;
    block->start_address = start;

    return block;
}

int *get_size(block_t *block) {
    return &block->size;
}

void process_ready(process_t *process, void *ready, int sim_time, char *mem_strategy, int (*insert)(void *, process_t *)) {
    set_state(process, READY);
    insert(ready, process);
    if (strcmp(mem_strategy, "best-fit") == 0) {
        printf("%d,READY,process_name=%s,assigned_at=%d\n", sim_time, get_name(process), get_block(process)->start_address);
    }

}

int compare_sizes(int *size1, int *size2) {

    if (*size1 > *size2) {
        return 1;
    } else if (*size1 < *size2) {
        return -1;
    } else {
        return 0;
    }
}
