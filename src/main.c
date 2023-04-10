/*
 * main.c - Contains the main() function and other general functions for the project
 * Author: Tristan Thomas
 * Date: 6-4-2023
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <unistd.h>

#include "process_data.h"
#include "process_scheduling.h"
#include "memory_allocation.h"
#include "linked_list.h"
#include "min_heap.h"

#define NUM_ARGS 9

enum state {
    IDLE, READY, RUNNING, FINISHED
};

void process_args(int argc, char **argv, char **scheduler, char **mem_strategy, int *quantum, FILE **file);
void cycle(int quantum, queue_t *processes, char *scheduler, char *mem_strategy);
void finish_process(process_t *process, queue_t *finished, int proc_remaining, int sim_time);
process_t *run_next_process(void *ready, int sim_time, process_t *(*extract)(void *), int (*is_empty)(void *));

int main(int argc, char *argv[]) {
    int quantum;
    char *scheduler = NULL, *mem_strategy = NULL;
    FILE *input_file = NULL;
    queue_t *processes = NULL;

    // process command line arguments
    process_args(argc, argv, &scheduler, &mem_strategy, &quantum, &input_file);

    // load processes into linked list
    processes = load_processes(processes, &input_file);

    cycle(quantum, processes, scheduler, mem_strategy);


    free(scheduler);
    scheduler = NULL;
    free(mem_strategy);
    free_list(processes, (void (*)(void *)) free_process);

    fclose(input_file);

    return 0;
}

void process_args(int argc, char **argv, char **scheduler, char **mem_strategy, int *quantum, FILE **file) {
    // check correct amount of arguments
    assert(argc == NUM_ARGS);
    int opt;

    while ((opt = getopt(argc, argv, "f:s:m:q:")) != -1) {
        switch (opt) {
            case 'm':
                *mem_strategy = strdup(optarg);
                assert(*mem_strategy);
                break;
            case 's':
                *scheduler = strdup(optarg);
                assert(*scheduler);
                break;
            case 'f':
                *file = fopen(optarg, "r");
                assert(*file);
                break;
            case 'q':
                *quantum = atoi(optarg);
                break;
            case '?':
                exit(EXIT_FAILURE);

        }

    }


}

void cycle(int quantum, queue_t *processes, char *scheduler, char *mem_strategy) {
    int sim_time = 0, num_cycles;
    queue_t *input_queue = create_empty_queue(), *finished_queue = create_empty_queue();
    void *ready_queue;
    int num_processes = get_queue_size(processes);
    process_t *current_process = NULL;

    int processes_remaining = 0;
    int run_new_process = 0;
    int no_process_running = 0;


    for (num_cycles = 0; ; num_cycles++) {

        // need to use min heap
        if (strcmp(scheduler, "SJF") == 0) {

            // for first cycle
            if (num_cycles == 0) {
                update_input(input_queue, processes, sim_time);
                ready_queue = create_heap();
                ready_queue = allocate_memory(input_queue, ready_queue, mem_strategy, (int (*)(void *, process_t *)) insert_process);
                current_process = run_next_process(ready_queue, sim_time, (process_t *(*)(void *)) extract_min, (int (*)(void *)) is_empty_heap);
                sim_time += quantum;
                continue;
            }

            // updates service time of current process and finishes process if finished
            if ((run_new_process = update_time(quantum, current_process))) {

                processes_remaining = get_queue_size(input_queue) + get_heap_size(ready_queue);
                finish_process(current_process, finished_queue, processes_remaining, sim_time);

                if (get_queue_size(finished_queue) == num_processes) {
                    break;
                }

            }
            // updates input queue
            update_input(input_queue, processes, sim_time);
            // updates ready queue

            ready_queue = allocate_memory(input_queue, ready_queue, mem_strategy, (int (*)(void *, process_t *)) insert_process);
            // starts new process
            if (run_new_process || (!is_empty_heap(ready_queue) && no_process_running)) {
                current_process = run_next_process(ready_queue, sim_time, (process_t *(*)(void *)) extract_min, (int (*)(void *)) is_empty_heap);
                no_process_running = (current_process == NULL) ? 1 : 0;
            }


            // need to use linked list
        } else if (strcmp(scheduler, "RR") == 0) {

            // for first cycle
            if (num_cycles == 0) {
                update_input(input_queue, processes, sim_time);
                ready_queue = create_empty_queue();
                ready_queue = allocate_memory(input_queue, ready_queue, mem_strategy,  (int (*)(void *, process_t *)) enqueue);
                current_process = run_next_process(ready_queue, sim_time, (process_t *(*)(void *)) dequeue, (int (*)(void *)) is_empty_queue);
                sim_time += quantum;
                continue;
            }

            // updates service time of current process and finishes process if finished
            if ((run_new_process = update_time(quantum, current_process))) {

                processes_remaining = get_queue_size(input_queue) + get_queue_size(ready_queue);

                finish_process(current_process, finished_queue, processes_remaining, sim_time);

                if (get_queue_size(finished_queue) == num_processes) {
                    break;
                }

            }

            // updates input queue
            update_input(input_queue, processes, sim_time);
            // updates ready queue
            ready_queue = allocate_memory(input_queue, ready_queue, mem_strategy, (int (*)(void *, process_t *)) enqueue);

            // suspend processes and decide which to run
            if (run_new_process) {
                current_process = run_next_process(ready_queue, sim_time, (process_t *(*)(void *)) dequeue, (int (*)(void *)) is_empty_queue);
            } else if (!is_empty_queue(ready_queue)) {
                enqueue(ready_queue, current_process);
                current_process = run_next_process(ready_queue, sim_time, (process_t *(*)(void *)) dequeue, (int (*)(void *)) is_empty_queue);

            }


        }


        // update simulation time
        sim_time += quantum;
    }

}

void finish_process(process_t *process, queue_t *finished, int proc_remaining, int sim_time) {
    set_state(process, FINISHED);
    enqueue(finished, process);
    printf("%d,FINISHED,process_name=%s,proc_remaining=%d\n", sim_time, get_name(process), proc_remaining);
}



process_t *run_next_process(void *ready, int sim_time, process_t *(*extract)(void *), int (*is_empty)(void *)) {
    if (is_empty(ready)) {
        return NULL;
    }
    process_t *current_process = extract(ready);

    set_state(current_process, RUNNING);
    printf("%d,RUNNING,process_name=%s,remaining_time=%d\n", sim_time, get_name(current_process), get_value(current_process, 's'));
    return current_process;
}




