/*
 * main.c - Contains the main() function and other general functions for the project
 * Author: Tristan Thomas
 * Date: 6-4-2023
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>

#include "process_data.h"
#include "process_scheduling.h"
#include "memory_allocation.h"
#include "linked_list.h"

#define NUM_ARGS 9
#define FILE_ARG 2
#define SCHEDULER_ARG 4
#define MEMORY_ARG 6
#define QUANTUM_ARG 8

void process_args(int argc, char **argv, char **scheduler, char **mem_strategy, int *quantum, FILE **file);

void cycle(int quantum, process_t **processes);

int main(int argc, char *argv[]) {
    int quantum;
    char *scheduler = NULL, *mem_strategy = NULL;
    FILE *input_file = NULL;
    processes_t *processes = NULL;

    // process command line arguments
    process_args(argc, argv, &scheduler, &mem_strategy, &quantum, &input_file);

    // load processes into linked list
    processes = load_processes(processes, &input_file);
    process_t *head = pop_head(processes);


    free(scheduler);
    free(mem_strategy);
    free_processes(processes);
    fclose(input_file);

    return 0;
}

void process_args(int argc, char **argv, char **scheduler, char **mem_strategy, int *quantum, FILE **file) {
    // check correct amount of arguments
    assert(argc == NUM_ARGS);
    *file = fopen(argv[FILE_ARG], "r");
    assert(*file);

    *scheduler = strdup(argv[SCHEDULER_ARG]);
    assert(*scheduler);
    *mem_strategy = strdup(argv[MEMORY_ARG]);
    assert(*mem_strategy);

    *quantum = atoi(argv[QUANTUM_ARG]);

}

void cycle(int quantum, process_t **processes) {
    int sim_time = 0;
    int num_cycles;

    for (num_cycles = 0; ; num_cycles++) {
        sim_time += quantum;


        break;
    }


}


