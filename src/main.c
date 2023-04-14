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
#include <stdint.h>
#include <math.h>
#include <signal.h>
#include <sys/types.h>
#include <sys/wait.h>



#include "process_data.h"
#include "process_scheduling.h"
#include "memory_allocation.h"
#include "linked_list.h"
#include "min_heap.h"

#define NUM_ARGS 9
#define READ 0
#define WRITE 1
#define IMPLEMENTS_REAL_PROCESS


enum state {
    IDLE, READY, RUNNING, FINISHED
};


void process_args(int argc, char **argv, char **scheduler, char **mem_strategy, int *quantum, FILE **file);
void cycle(int quantum, list_t *processes, char *scheduler, char *mem_strategy);
void finish_process(process_t *process, list_t *finished, list_t *memory, list_t *holes, int proc_remaining, int sim_time, char *mem_strategy);
process_t *run_next_process(void *ready, int sim_time, process_t *(*extract)(void *), int (*is_empty)(void *));
void print_statistics(list_t *finished, int makespan);
double mean(list_t *list, char field);
double max(list_t *list, char field);
void run_real_process(process_t *process, int sim_time);
uint8_t send_bytes(process_t *process, int num);
void read_and_verify(process_t *process, uint8_t test_byte);
void suspend_process(process_t *process, int sim_time);
void continue_process(process_t *process, int sim_time);



int main(int argc, char *argv[]) {
    int quantum;
    char *scheduler = NULL, *mem_strategy = NULL;
    FILE *input_file = NULL;
    list_t *processes = NULL;

    // process command line arguments
    process_args(argc, argv, &scheduler, &mem_strategy, &quantum, &input_file);

    // load processes into linked list
    processes = load_processes(processes, &input_file);

    cycle(quantum, processes, scheduler, mem_strategy);


    free(scheduler);
    scheduler = NULL;
    free(mem_strategy);
    mem_strategy = NULL;
    free_list(processes, (void (*)(void *)) free_process);

    fclose(input_file);
    input_file = NULL;

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

void cycle(int quantum, list_t *processes, char *scheduler, char *mem_strategy) {
    int sim_time = 0, num_cycles;
    list_t *input_queue = create_empty_list(), *finished_queue = create_empty_list();
    void *ready_queue;
    int num_processes = get_list_size(processes);
    process_t *current_process = NULL;
    // file descriptors for pipe


    int processes_remaining;
    int new_process;
    int no_process_running = 0;
    list_t *memory = create_empty_list();
    list_t *holes = create_empty_list();
    initialise_memory(memory, holes);


    for (num_cycles = 0; ; num_cycles++) {

        // need to use min heap
        if (strcmp(scheduler, "SJF") == 0) {

            // for first cycle
            if (num_cycles == 0) {
                update_input(input_queue, processes, sim_time);
                ready_queue = create_heap();
                ready_queue = allocate_memory(memory, holes, input_queue, ready_queue, mem_strategy, sim_time, (int (*)(void *, process_t *)) insert_process);
                current_process = run_next_process(ready_queue, sim_time, (process_t *(*)(void *)) extract_min, (int (*)(void *)) is_empty_heap);
                sim_time += quantum;
                continue;
            }

            // updates service time of current process and finishes process if finished
            if ((new_process = update_time(quantum, current_process))) {

                processes_remaining = get_list_size(input_queue) + get_heap_size(ready_queue);
                finish_process(current_process, finished_queue, memory, holes, processes_remaining, sim_time, mem_strategy);
                no_process_running = 1;
                if (get_list_size(finished_queue) == num_processes) {
                    break;
                }

            }
            // updates input queue
            update_input(input_queue, processes, sim_time);
            // updates ready queue
            if (!no_process_running) {

                continue_process(current_process, sim_time);

            }
            ready_queue = allocate_memory(memory, holes, input_queue, ready_queue, mem_strategy, sim_time, (int (*)(void *, process_t *)) insert_process);
            // starts new process
            if (new_process || (!is_empty_heap(ready_queue) && no_process_running)) {
                current_process = run_next_process(ready_queue, sim_time, (process_t *(*)(void *)) extract_min, (int (*)(void *)) is_empty_heap);
                no_process_running = (current_process == NULL) ? 1 : 0;
            }


            // need to use linked list
        } else if (strcmp(scheduler, "RR") == 0) {

            // for first cycle
            if (num_cycles == 0) {
                update_input(input_queue, processes, sim_time);
                ready_queue = create_empty_list();
                ready_queue = allocate_memory(memory, holes, input_queue, ready_queue, mem_strategy, sim_time, (int (*)(void *, process_t *)) enqueue);
                current_process = run_next_process(ready_queue, sim_time, (process_t *(*)(void *)) dequeue, (int (*)(void *)) is_empty_list);
                sim_time += quantum;
                continue;
            }

            // updates service time of current process and finishes process if finished
            if ((new_process = update_time(quantum, current_process))) {

                processes_remaining = get_list_size(input_queue) + get_list_size(ready_queue);

                finish_process(current_process, finished_queue, memory, holes, processes_remaining, sim_time, mem_strategy);

                if (get_list_size(finished_queue) == num_processes) {
                    break;
                }

            }

            // updates input queue
            update_input(input_queue, processes, sim_time);
            // updates ready queue
            ready_queue = allocate_memory(memory, holes, input_queue, ready_queue, mem_strategy, sim_time, (int (*)(void *, process_t *)) enqueue);

            // suspend processes and decide which to run
            if (new_process) {
                current_process = run_next_process(ready_queue, sim_time, (process_t *(*)(void *)) dequeue, (int (*)(void *)) is_empty_list);
            } else if (!is_empty_list(ready_queue)) {
                enqueue(ready_queue, current_process);
                suspend_process(current_process, sim_time);
                current_process = run_next_process(ready_queue, sim_time, (process_t *(*)(void *)) dequeue, (int (*)(void *)) is_empty_list);
            } else {
                continue_process(current_process, sim_time);
            }


        }


        // update simulation time
        sim_time += quantum;
    }

    print_statistics(finished_queue, sim_time);

    free_list(finished_queue, (void (*)(void *)) free_process);
    free_list(input_queue, (void (*)(void *)) free_process);
    free_list(memory, (void (*)(void *)) free);
    free_list(holes, blank);

    if (strcmp(scheduler, "RR") == 0) {
        free_list(ready_queue, (void (*)(void *)) free_process);
    } else if (strcmp(scheduler, "SJF") == 0) {
        free_heap(ready_queue);
    }

}

void suspend_process(process_t *process, int sim_time) {

    send_bytes(process, sim_time);
    int wstatus, w;

    kill(get_value(process, 'p'), SIGTSTP);

    do {
        w = waitpid(get_value(process, 'p'), &wstatus, WUNTRACED);
        if (w == -1) {
            perror("waitpid");
            exit(EXIT_FAILURE);
        }

    } while (!WIFSTOPPED(wstatus));



}

void continue_process(process_t *process, int sim_time) {

    int8_t test_byte = send_bytes(process, sim_time);
    kill(get_value(process, 'p'), SIGCONT);
    read_and_verify(process, test_byte);

}

void finish_process(process_t *process, list_t *finished, list_t *memory, list_t *holes, int proc_remaining, int sim_time, char *mem_strategy) {
    char sha256[64];

    set_state(process, FINISHED);
    enqueue(finished, process);
    printf("%d,FINISHED,process_name=%s,proc_remaining=%d\n", sim_time, get_name(process), proc_remaining);

    // terminate
    send_bytes(process, sim_time);
    kill(get_value(process, 'p'), SIGTERM);
    read(get_fd_in(process)[READ], sha256, 64);

    printf("%d,FINISHED-PROCESS,process_name=%s,sha=", sim_time, get_name(process));
    for (int i = 0; i < 64; i++) {
        printf("%c", sha256[i]);
    }
    printf("\n");

    set_value(process, sim_time, 'f');
    update_stats(process);
    deallocate_memory(process, memory, holes, mem_strategy);
}



process_t *run_next_process(void *ready, int sim_time, process_t *(*extract)(void *), int (*is_empty)(void *)) {

    if (is_empty(ready)) {
        return NULL;
    }
    process_t *current_process = extract(ready);

    if (get_value(current_process, 'p') == 0) {
        run_real_process(current_process, sim_time);
    } else {
        continue_process(current_process, sim_time);
    }


    set_state(current_process, RUNNING);
    printf("%d,RUNNING,process_name=%s,remaining_time=%d\n", sim_time, get_name(current_process), (int) get_value(current_process, 'l'));
    return current_process;
}




void run_real_process(process_t *process, int sim_time) {
    int fd_out[2], fd_in[2];
    pid_t child_pid;

    if (pipe(fd_out) == -1 || pipe(fd_in) == -1) {
        perror("pipe");
        exit(EXIT_FAILURE);
    }

    if ((child_pid = fork()) == -1) {
        perror("fork");
        exit(1);
    }

    if (child_pid == 0) {
        close(fd_out[WRITE]); // close write
        close(fd_in[READ]); //close read

        dup2(fd_out[READ], STDIN_FILENO);
        dup2(fd_in[WRITE], STDOUT_FILENO);

        char *args[] = {"process", get_name(process), NULL};
        // replace the child process with process program
        execv(args[0], args);
        exit(0);

    } else {

        close(fd_out[READ]); // close read
        close(fd_in[WRITE]); // close write
        set_fds(process, fd_in, fd_out);
        // create
        uint8_t test_byte = send_bytes(process, sim_time);
        read_and_verify(process, test_byte);


    }

    set_value(process, child_pid, 'p');
    // parent program will continue
}

void read_and_verify(process_t *process, uint8_t test_byte)  {

    uint8_t buf[1];
    if (read(get_fd_in(process)[READ], buf, 1) != 1) {
        perror("read");
        exit(EXIT_FAILURE);
    }

    if (buf[0] != test_byte) {
        exit(EXIT_FAILURE);
    }

}

uint8_t send_bytes(process_t *process, int num) {

    uint8_t time_bytes[4];
    *(uint32_t *) time_bytes = (uint32_t) num;
    // big endian (MSB in lowest mem address)
    for (int i = 3; i >= 0; i--) {
        if (write(get_fd_out(process)[WRITE], &time_bytes[i], 1) != 1) {
            perror("write");
            exit(EXIT_FAILURE);
        }
    }

    return time_bytes[0];

}









void print_statistics(list_t *finished, int makespan) {
    int avg_turnaround;
    double avg_overhead, max_overhead;
    avg_turnaround = ceil(mean(finished, 't'));
    // rounded to remove effects of floating point arithmetic
    avg_overhead = round(mean(finished, 'o') * 100) / 100;
    max_overhead = round(max(finished, 'o') * 100) / 100;

    printf("Turnaround time %d\n", avg_turnaround);
    printf("Time overhead %.2lf %.2lf\n", max_overhead, avg_overhead);
    printf("Makespan %d\n", makespan);
}

double mean(list_t *list, char field) {

    double sum = 0;
    node_t *curr = get_head(list);
    process_t *curr_proc;
    while (curr != NULL) {

        curr_proc = (process_t *) get_data(curr);
        sum += get_value(curr_proc, field);

        curr = get_next(curr);
    }

    return (double) sum / get_num_items(list);

}

double max(list_t *list, char field) {

    node_t *curr = get_head(list);
    process_t *curr_proc = (process_t *) get_data(curr);;
    double max = get_value(curr_proc, field);

    while (curr != NULL) {

        curr_proc = (process_t *) get_data(curr);
        if (get_value(curr_proc, field) > max) {
            max = get_value(curr_proc, field);
        }

        curr = get_next(curr);

    }

    return max;

}




