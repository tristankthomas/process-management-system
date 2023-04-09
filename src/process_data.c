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

enum state {
    IDLE, READY, RUNNING, FINISHED
};

struct process {
    int arrival_time, service_time, mem_requirement;
    char* name;
    state_t state;

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
        process->arrival_time = time_arrived;
        process->name = strdup(name);
    } else {
        process = NULL;
    }

    return process;

}

queue_t *load_processes(queue_t *processes, FILE **file) {

    processes = create_empty_queue();

    // adds process to linked list until EOF is reached
    while (enqueue(processes, read_process(file)));

    return processes;
}

void free_process(process_t *process) {

    free(process->name);
    free(process);
}


int get_value(process_t *process, char field) {
    switch (field) {
        case 'a':
            return process->arrival_time;
        case 's':
            return process->service_time;
        case 'm':
            return process->mem_requirement;
        default:
            exit(EXIT_FAILURE);
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
        case 'm':
            process->mem_requirement = value;
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