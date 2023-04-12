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
#include "memory_allocation.h"

enum state {
    IDLE, READY, RUNNING, FINISHED
};

enum block_type {
    PROCESS, HOLE
};

struct process {
    int arrival_time, service_time, service_time_left, mem_requirement, finish_time, turnaround_time;
    double overhead;
    char* name;
    state_t state;
    list_node_t *block_node;

};






process_t *read_process(FILE **file) {
    process_t *process = (process_t *) malloc(sizeof(*process));
    assert(process);

    char name[MAX_NAME_LEN] = "";
    int service_time = 0, time_arrived = 0, mem_requirement = 0;

    if (fscanf(*file, "%d %s %d %d", &time_arrived, name, &service_time, &mem_requirement) != EOF) {
        process->state = IDLE;
        process->mem_requirement = mem_requirement;
        process->service_time = service_time;
        process->service_time_left = service_time;
        process->arrival_time = time_arrived;
        process->name = strdup(name);
    } else {
        process = NULL;
    }


    return process;

}

list_t *load_processes(list_t *processes, FILE **file) {

    processes = create_empty_list();

    // adds process to linked list until EOF is reached
    while (enqueue(processes, read_process(file)));

    return processes;
}

void free_process(process_t *process) {

    free(process->name);
    free(process);
}


double get_value(process_t *process, char field) {
    switch (field) {
        case 'a':
            return process->arrival_time;
        case 's':
            return process->service_time;
        case 'l':
            return process->service_time_left;
        case 'm':
            return process->mem_requirement;
        case 'f':
            return process->finish_time;
        case 't':
            return process->turnaround_time;
        case 'o':
            return process->overhead;
        default:
            exit(EXIT_FAILURE);
    }
}


void update_stats(process_t *process) {
    process->turnaround_time = process->finish_time - process->arrival_time;
    process->overhead = (double) process->turnaround_time / process->service_time;
}

int update_time(int quantum, process_t *process) {
    if (process == NULL) {
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

char *get_name(process_t *process) {
    return process->name;
}

void set_value(process_t *process, int value, char field) {
    switch (field) {
        case 'a':
            process->arrival_time = value;
            break;
        case 's':
            process->service_time = value;
            break;
        case 'l':
            process->service_time_left = value;
            break;
        case 'm':
            process->mem_requirement = value;
            break;
        case 'f':
            process->finish_time = value;
            break;
        case 't':
            process->turnaround_time = value;
            break;
        case 'o':
            process->overhead = value;
            break;
        default:
            exit(EXIT_FAILURE);

    }

}

int cmp_service_time(process_t *p1, process_t *p2) {

    if (p1->service_time < p2->service_time) {
        return 0;
    } else if (p1->service_time > p2->service_time) {
        return 1;
    } else {
        return 0;
    }
}

void set_state(process_t *process, state_t state) {
    process->state = state;
}

void set_block_node(process_t *process, list_node_t *block_node) {
    process->block_node = block_node;
}

list_node_t *get_block_node(process_t *process) {
    return process->block_node;
}