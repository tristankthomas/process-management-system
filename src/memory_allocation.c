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

        list_node_t *curr = get_head(input);

        while (curr != NULL) {
            process = (process_t *) get_data(curr);

            if (best_fit(holes, memory, process)) {
                process_ready(process, ready, sim_time, mem_strategy, insert);
                dequeue(input);
            }

            curr = get_next(curr);
        }


    }

    return ready;
}


int best_fit(list_t *holes, list_t *memory, process_t *process) {
    int is_space;
    list_node_t *curr = get_head(holes);
    list_node_t *best_fit = NULL;

    // try to refactor this!
    while ((is_space = (curr != NULL))) {
        block_t *curr_block = (block_t *) get_data(get_data(curr));
        // keep iterating until block is bigger than or equal to what is needed (will be best-fit since sorted from smallest to largest)
        if (curr_block->size >= (int) get_value(process, 'm')) {
            best_fit = curr;
            break;
        }
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
    int new_size = (int) get_value(process, 'm');
    int start_address = old_block->start_address;
    // block stored in new node
    block_t *new_block = create_block(PROCESS, start_address, new_size);

    old_block->type = HOLE;
    old_block->start_address = start_address + new_size;
    old_block->size = prev_size - new_size;

    list_node_t *prev_node = get_prev(node);
    if (old_block->size == 0) {
        // free node
        node = NULL;
        // node essentially deleted
        set_num_items(memory, get_num_items(memory) - 1);
    } else {
        insert_node_sorted(holes, node, (int (*)(void *, void *)) compare_sizes, (void *(*)(void *)) get_size);
    }

    list_node_t *new_node = insert_list_node(memory, new_block, prev_node, node);
    set_block_node(process, new_node);

}

void deallocate_memory(process_t *process, list_t *memory, list_t *holes, char *mem_strategy) {
    if (strcmp(mem_strategy, "infinity") == 0) {

        return;

    } else if (strcmp(mem_strategy, "best-fit") == 0) {
        list_node_t *block_node = get_block_node(process);

        ((block_t *) get_data(block_node))->type = HOLE;

        // check to right
        if (get_next(block_node) != NULL) {
            check_direction(memory, holes, block_node, get_next);
        }


        // check to left
        if (get_prev(block_node) != NULL) {
            check_direction(memory, holes, block_node, get_prev);
        }
        // need to delete holes from holes list when updating memory
        insert_node_sorted(holes, block_node, (int (*)(void *, void *)) compare_sizes, (void *(*)(void *)) get_size);
        set_block_node(process, NULL);


    }

}

void check_direction(list_t *memory, list_t *holes, list_node_t *block_node, list_node_t *(*get_dir)(list_node_t *)) {

    list_node_t *curr = get_dir(block_node);
    block_t *curr_block = (block_t *) get_data(curr);

    while (curr_block->type == HOLE) {
        update_memory(memory, holes, block_node, curr);
        curr = get_dir(curr);
        if (curr == NULL) {
            break;
        }
        curr_block = (block_t *) get_data(curr);
    }
}

void update_memory(list_t *memory, list_t *holes, list_node_t *main_node, list_node_t *adj_node) {
    block_t *adj_block = (block_t *) get_data(adj_node);
    block_t *main_block = (block_t *) get_data(main_node);
    int adj_start_address = adj_block->start_address;
    int main_start_address = main_block->start_address;
    // delete the adjacent node from holes too - do this by searching through holes and finding block address that matches adj_block and then delete and free this (ie delete node based on data)
    main_block->start_address = (adj_start_address < main_start_address) ? adj_start_address : main_start_address;
    main_block->size += adj_block->size;
    delete_node(memory, adj_node);
    delete_node_by_data(holes, adj_node);
    // maybe free later
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
        printf("%d,READY,process_name=%s,assigned_at=%d\n", sim_time, get_name(process), ((block_t *) get_data(get_block_node(process)))->start_address);
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
