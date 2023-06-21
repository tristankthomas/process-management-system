/*
 * memory_allocation.c - Contains the functions and data structures for memory allocation
 * Author: Tristan Thomas
 * Date: 6-4-2023
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>

#include "memory_allocation.h"
#include "linked_list.h"
#include "process_data.h"

/* Definitions of memory block and block type enum */
enum block_type {
    PROCESS, HOLE
};

struct block {
    block_type_t type;
    int start_address;
    int size;
};

typedef node_t *(*get_dir_func)(node_t *);

static int best_fit(list_t *holes, list_t *memory, process_t *process);
static void split(process_t *process, node_t *node, list_t *holes, list_t *memory);
static void process_ready(process_t *process, void *ready, uint32_t sim_time, char *mem_strategy, insert_func insert);
static void update_memory(list_t *memory, list_t *holes, node_t *main_node, node_t *adj_node);
static void check_direction(list_t *memory, list_t *holes, node_t *block_node, get_dir_func get_dir);
static block_t *create_block(block_type_t type, int start, int size);


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
                      insert_func insert) {
    process_t *process;
    if (strcmp(mem_strategy, "infinite") == 0) {

        // transfer the process straight to ready
        while ((process = dequeue(input)) != NULL) {
            process_ready(process, ready, sim_time, mem_strategy, insert);
        }

    } else if (strcmp(mem_strategy, "best-fit") == 0) {

        node_t *curr = get_head(input);
        node_t *next;
        int is_space;

        while (curr) {
            process = (process_t *) get_data(curr);

            if ((is_space = best_fit(holes, memory, process))) {
                process_ready(process, ready, sim_time, mem_strategy, insert);
                delete_node(input, curr);
            }
            next = get_next(curr);
            if (is_space) {
                free(curr);
                curr = NULL;
            }
            curr = next;
        }


    }

    return ready;
}

/**
 * Checks if there is room in memory and if so allocates this memory
 *
 * @param holes List of holes (nodes of memory)
 * @param memory Memory allocation list
 * @param process Process to be allocated
 * @return 1 if there is space, 0 if not
 */
static int best_fit(list_t *holes, list_t *memory, process_t *process) {

    int is_space;
    node_t *curr = get_head(holes);
    node_t *best_fit = NULL;
    node_t *mem_node = NULL;

    while ((is_space = (curr != NULL))) {
        block_t *curr_block = (block_t *) get_data(get_data(curr));
        // keep iterating until block is bigger than or equal to what is needed
        if (curr_block->size >= (int) get_value(process, MEMORY_REQUIREMENT)) {
            best_fit = curr;
            break;
        }
        curr = get_next(curr);
    }

    // allocates memory
    if (is_space) {
        delete_node(holes, best_fit);
        mem_node = get_data(best_fit);
        free(best_fit);
        best_fit = NULL;
        split(process, mem_node, holes, memory);
    }

    return is_space;

}

/**
 * Splits the free memory hole
 *
 * @param process Process to be allocated
 * @param node Hole node to be split
 * @param holes List of holes
 * @param memory Memory allocation list
 */
static void split(process_t *process, node_t *node, list_t *holes, list_t *memory) {

    // memory block from previous node
    block_t *old_block = (block_t *) get_data(node);

    int prev_size = old_block->size;
    int new_size = (int) get_value(process, MEMORY_REQUIREMENT);
    int start_address = old_block->start_address;
    // block stored in new node
    block_t *new_block = create_block(PROCESS, start_address, new_size);

    old_block->type = HOLE;
    old_block->start_address = start_address + new_size;
    old_block->size = prev_size - new_size;

    node_t *prev_node = get_prev(node);

    // process fits perfectly in hole
    if (old_block->size == 0) {

        delete_node(memory, node);
        node_t *temp = node;
        node = get_next(node);
        free_node(temp, free);

    } else {
        // inserts the same node (but reduced in size) that was deleted in best_fit
        insert_node_sorted(holes, node, (compare_func) compare_ints, (get_sort_value_func) get_size);
    }

    node_t *new_node = insert_node(memory, new_block, prev_node, node);
    set_block_node(process, new_node);

}

/**
 * Deallocates the memory for a process once it is terminated
 *
 * @param process Process to be deallocated
 * @param memory Memory management list
 * @param holes List of holes
 * @param mem_strategy Memory strategy
 */
void deallocate_memory(process_t *process, list_t *memory, list_t *holes, char *mem_strategy) {

    if (strcmp(mem_strategy, "infinity") == 0) {

        return;

    } else if (strcmp(mem_strategy, "best-fit") == 0) {

        node_t *block_node = get_block_node(process);

        ((block_t *) get_data(block_node))->type = HOLE;

        // check to right
        if (get_next(block_node)) {
            check_direction(memory, holes, block_node, (get_dir_func) get_next);
        }

        // check to left
        if (get_prev(block_node)) {
            check_direction(memory, holes, block_node, (get_dir_func) get_prev);
        }
        // need to delete holes from holes list when updating memory
        insert_node_sorted(holes, block_node, (compare_func) compare_ints, (get_sort_value_func) get_size);
        set_block_node(process, NULL);


    }

}

/**
 * Checks the adjacent memory blocks and merges if holes
 *
 * @param memory Memory management list
 * @param holes List of holes
 * @param block_node Starting memory node
 * @param get_dir Function that determines which direction (left or right) to traverse
 */
static void check_direction(list_t *memory, list_t *holes, node_t *block_node, get_dir_func get_dir) {

    node_t *curr = get_dir(block_node);
    node_t *next;
    block_t *curr_block = (block_t *) get_data(curr);

    while (curr_block->type == HOLE) {
        next = get_dir(curr);
        // merge hole with block node
        update_memory(memory, holes, block_node, curr);

        curr = next;

        if (!curr) {
            break;
        }

        curr_block = (block_t *) get_data(curr);

    }
}

/**
 * Merges the block node with the adjacent hole and updates memory
 *
 * @param memory Memory management list
 * @param holes List of holes
 * @param main_node Memory block to be increased
 * @param adj_node Adjacent memory block to be merged
 */
static void update_memory(list_t *memory, list_t *holes, node_t *main_node, node_t *adj_node) {

    block_t *adj_block = (block_t *) get_data(adj_node);
    block_t *main_block = (block_t *) get_data(main_node);
    int adj_start_address = adj_block->start_address;
    int main_start_address = main_block->start_address;

    main_block->start_address = (adj_start_address < main_start_address) ? adj_start_address : main_start_address;
    main_block->size += adj_block->size;

    // deletes the adjacent node from holes list and memory
    delete_node(memory, adj_node);
    delete_node_by_data(holes, adj_node, free);

}


/**
 * Initialises memory and holes to 2048 Bytes
 *
 * @param memory Memory management list
 * @param holes List of holes
 */
void initialise_memory(list_t *memory, list_t *holes) {

    block_t *init_hole = create_block(HOLE, 0, MEM_CAPACITY);
    enqueue(memory, init_hole);
    enqueue(holes, get_head(memory));
}

/**
 * Creates a memory block
 *
 * @param type Type of memory block (hole or process)
 * @param start Starting memory address
 * @param size Size of memory block in Bytes
 * @return The newly created block
 */
static block_t *create_block(block_type_t type, int start, int size) {

    block_t *block = malloc(sizeof(*block));
    assert(block);

    block->type = type;
    block->size = size;
    block->start_address = start;

    return block;
}

/**
 * Adds the process to ready queue and prints status
 *
 * @param process Ready process
 * @param ready Ready queue
 * @param sim_time Current simulation time
 * @param mem_strategy Memory Strategy
 * @param insert Insertion function for ready queue depending on scheduling algorithm
 */
static void process_ready(process_t *process, void *ready, uint32_t sim_time, char *mem_strategy, insert_func insert) {

    set_state(process, READY);
    insert(ready, process);
    if (strcmp(mem_strategy, "best-fit") == 0) {
        printf("%u,READY,process_name=%s,assigned_at=%d\n", sim_time, get_name(process),
               ((block_t *) get_data(get_block_node(process)))->start_address);
    }

}

/**
 * Compares two ints - used to compare block sizes when inserting block into holes list
 *
 * @param int1 Pointer to int
 * @param int2 Pointer to int
 * @return Returns 1, 0 or -1 depending on relative int values
 */
int compare_ints(int *int1, int *int2) {

    if (*int1 > *int2) {
        return 1;
    } else if (*int1 < *int2) {
        return -1;
    } else {
        return 0;
    }
}

/**
 * Gets size of block
 *
 * @param block Specified block
 * @return Size of block
 */
int *get_size(block_t *block) {
    return &block->size;
}
