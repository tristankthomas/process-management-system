/*
 * process_data.c - Contains the data structure and functions to store and manipulate a process
 * Author: Tristan Thomas
 * Date: 6-4-2023
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>

#include "process_data.h"
#include "linked_list.h"

/* Definition of a process */
struct process {
    int arrival_time, service_time, service_time_left, mem_requirement, finish_time, turnaround_time;
    double overhead;
    char* name;
    state_t state;
    node_t *block_node;
    pid_t pid;
    int fd_out[2],fd_in[2];
};

static process_t *read_process(FILE **file);


/**
 * Loads all processes into linked list from a file
 *
 * @param processes List of processes
 * @param file File to be read from
 * @return Updated list of processes
 */
list_t *load_processes(list_t *processes, FILE **file) {

    processes = create_empty_list();

    // adds process to linked list until EOF is reached
    while (enqueue(processes, read_process(file)));

    return processes;
}

/**
 * Reads a single process from a file
 *
 * @param file File to be read from
 * @return New process
 */
static process_t *read_process(FILE **file) {

    char name[MAX_NAME_LEN] = "";
    int service_time = 0, time_arrived = 0, mem_requirement = 0;
    process_t *process = NULL;

    if (fscanf(*file, "%d %s %d %d", &time_arrived, name, &service_time, &mem_requirement) != EOF) {

        process = malloc(sizeof(*process));
        assert(process);

        process->state = IDLE;
        process->mem_requirement = mem_requirement;
        process->service_time = service_time;
        process->service_time_left = service_time;
        process->arrival_time = time_arrived;
        process->name = strdup(name);
        process->block_node = NULL;
        process->pid = 0;

    }

    return process;

}

/**
 * Frees a process
 * @param process Process to be freed
 */
void free_process(process_t *process) {

    free(process->name);
    process->name = NULL;
    free(process);
    process = NULL;
}

/**
 * Gets a value from a process
 *
 * @param process Specified process
 * @param field Value type to return
 * @return Value
 */
double get_value(process_t *process, enum value field) {

    switch (field) {
        case ARRIVAL_TIME:
            return process->arrival_time;
        case SERVICE_TIME:
            return process->service_time;
        case SERVICE_TIME_LEFT:
            return process->service_time_left;
        case MEMORY_REQUIREMENT:
            return process->mem_requirement;
        case FINISH_TIME:
            return process->finish_time;
        case TURNAROUND_TIME:
            return process->turnaround_time;
        case OVERHEAD:
            return process->overhead;
        case PID:
            return process->pid;
        default:
            exit(EXIT_FAILURE);
    }
}

/**
 * Updates turnaround time and overhead of a process
 *
 * @param process Process to be updated
 */
void update_stats(process_t *process) {

    process->turnaround_time = process->finish_time - process->arrival_time;
    process->overhead = (double) process->turnaround_time / process->service_time;
}

/**
 * Updates service time of a process
 *
 * @param quantum Amount of time to be subtracted
 * @param process Process to be updated
 * @return 1 if process is finished, 0 otherwise
 */
int update_time(int quantum, process_t *process) {

    if (!process) {
        return 0;
    }

    if (process->service_time_left - quantum <= 0) {
        process->service_time_left = 0;
        process->state = FINISHED;
        return 1;
    } else {
        process->service_time_left -= quantum;
        return 0;
    }

}

/**
 * Gets process name
 *
 * @param process Specified process
 * @return Process name
 */
char *get_name(process_t *process) {

    return process->name;
}

/**
 * Sets a value of a process
 *
 * @param process Process to be updated
 * @param value Integer value
 * @param field Required field to update
 */
void set_value(process_t *process, int value, enum value field) {

    switch (field) {
        case ARRIVAL_TIME:
            process->arrival_time = value;
            break;
        case SERVICE_TIME:
            process->service_time = value;
            break;
        case SERVICE_TIME_LEFT:
            process->service_time_left = value;
            break;
        case MEMORY_REQUIREMENT:
            process->mem_requirement = value;
            break;
        case FINISH_TIME:
            process->finish_time = value;
            break;
        case TURNAROUND_TIME:
            process->turnaround_time = value;
            break;
        case PID:
            process->pid = value;
            break;
        default:
            exit(EXIT_FAILURE);

    }

}

/**
 * Compares service time used for min heap sorting
 * @param p1 Process 1
 * @param p2 Process 2
 * @return An int based on comparison property
 */
int cmp_service_time(process_t *p1, process_t *p2) {

    if (p1->service_time < p2->service_time) {
        return -1;
    } else if (p1->service_time > p2->service_time) {
        return 1;
    } else {
        // lexicographical order of name
        return strcmp(p1->name, p2->name);
    }
}

/**
 * Sets state of process
 *
 * @param process Process to be updated
 * @param state New state
 */
void set_state(process_t *process, state_t state) {

    process->state = state;
}

/**
 * Sets the memory block of a process (as a node of memory list)
 *
 * @param process Process to be updated
 * @param block_node Memory block node from memory
 */
void set_block_node(process_t *process, node_t *block_node) {

    process->block_node = block_node;
}

/**
 * Gets the block node from a process
 *
 * @param process Specified process
 * @return Block node
 */
node_t *get_block_node(process_t *process) {

    return process->block_node;
}

/**
 * Sets the file descriptor pipe pairs of a process
 *
 * @param process Specified process
 * @param fd_in File descriptor pair for reading from real process
 * @param fd_out File descriptor pair for writing to real process
 */
void set_fds(process_t *process, int fd_in[], int fd_out[]) {

    process->fd_in[READ] = fd_in[READ];
    process->fd_in[WRITE] = fd_in[WRITE];
    process->fd_out[READ] = fd_out[READ];
    process->fd_out[WRITE] = fd_out[WRITE];

}

/**
 * Gets file descriptor pair for reading from real process
 * @param process Specified process
 * @return File descriptor pair
 */
int *get_fd_in(process_t *process) {

    return process->fd_in;
}

/**
 * Gets file descriptor pair for writing to real process
 * @param process Specified process
 * @return File descriptor pair
 */
int *get_fd_out(process_t *process) {

    return process->fd_out;
}
