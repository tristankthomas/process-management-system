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
#include <sys/wait.h>

#include "process_data.h"
#include "memory_allocation.h"
#include "linked_list.h"
#include "min_heap.h"

#define NUM_ARGS 9
#define HASH_SIZE 64

#define IMPLEMENTS_REAL_PROCESS

void process_args(int argc, char **argv, char **scheduler, char **mem_strategy, int *quantum, FILE **file);
void cycle(int quantum, list_t *processes, char *scheduler, char *mem_strategy);
void finish_process(process_t *process, list_t *finished, list_t *memory, list_t *holes, int proc_remaining,
                    int sim_time, char *mem_strategy);
process_t *run_next_process(void *ready, int sim_time, process_t *(*extract)(void *), int (*is_empty)(void *));
void print_statistics(list_t *finished, int makespan);
double mean(list_t *list, enum value field);
double max(list_t *list, enum value field);
void start_real_process(process_t *process, int sim_time);
uint8_t send_bytes(process_t *process, int num);
void read_and_verify(process_t *process, uint8_t test_byte);
void suspend_process(process_t *process, int sim_time);
void continue_process(process_t *process, int sim_time);
list_t *update_input(list_t *input, list_t *processes, int sim_time);


/**
 * Main entry point of program
 *
 * @param argc Number of command line arguments
 * @param argv Array of command line arguments
 * @return 0 upon successful exit
 */
int main(int argc, char *argv[]) {

    int quantum;
    char *scheduler = NULL, *mem_strategy = NULL;
    FILE *input_file = NULL;
    list_t *processes = NULL;

    // process command line arguments
    process_args(argc, argv, &scheduler, &mem_strategy, &quantum, &input_file);

    // load processes into linked list
    processes = load_processes(processes, &input_file);

    // completes processes
    cycle(quantum, processes, scheduler, mem_strategy);

    // frees allocated memory
    free(scheduler);
    scheduler = NULL;
    free(mem_strategy);
    mem_strategy = NULL;
    free_list(processes, (void (*)(void *)) free_process);

    fclose(input_file);
    input_file = NULL;

    return 0;
}

/**
 * Processes command line arguments
 *
 * @param argc Number of arguments
 * @param argv Array of arguments
 * @param scheduler Scheduler type
 * @param mem_strategy Memory strategy
 * @param quantum Amount of time per cycle
 * @param file File to be read from
 */
void process_args(int argc, char **argv, char **scheduler, char **mem_strategy, int *quantum, FILE **file) {

    int opt;
    // check correct amount of arguments
    assert(argc == NUM_ARGS);

    // Reads command line flags and values
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

/**
 * Simulates the scheduling and completion of processes using either shortest job first or round robin algorithms
 *
 * @param quantum Amount of time per cycle
 * @param processes List of processes to be ran
 * @param scheduler Scheduling algorithm to be used
 * @param mem_strategy Memory strategy to be used
 */
void cycle(int quantum, list_t *processes, char *scheduler, char *mem_strategy) {

    int sim_time = 0, num_cycles, processes_remaining, new_process, no_process_running = 0;
    int num_processes = get_list_size(processes);
    // process lists
    list_t *input_queue = create_empty_list(), *finished_queue = create_empty_list();
    void *ready_queue;
    // memory lists
    list_t *memory = create_empty_list();
    list_t *holes = create_empty_list();
    initialise_memory(memory, holes);
    process_t *current_process = NULL;


    for (num_cycles = 0; ; num_cycles++) {

        // shortest job first algorithm - uses min heap for ready queue
        if (strcmp(scheduler, "SJF") == 0) {

            // for first cycle
            if (num_cycles == 0) {

                update_input(input_queue, processes, sim_time);
                ready_queue = create_heap();
                ready_queue = allocate_memory(memory, holes, input_queue, ready_queue, mem_strategy,
                                              sim_time, (int (*)(void *, process_t *)) insert_process);
                current_process = run_next_process(ready_queue, sim_time, (process_t *(*)(void *)) extract_min,
                                                   (int (*)(void *)) is_empty_heap);

                sim_time += quantum;
                continue;
            }

            // updates service time of current process and finishes process if finished
            if ((new_process = update_time(quantum, current_process))) {

                processes_remaining = get_list_size(input_queue) + get_heap_size(ready_queue);
                finish_process(current_process, finished_queue, memory, holes, processes_remaining,
                               sim_time, mem_strategy);
                no_process_running = 1;
                if (get_list_size(finished_queue) == num_processes) {
                    // all processes completed
                    break;
                }

            }

            // updates input queue
            update_input(input_queue, processes, sim_time);

            if (!no_process_running) {
                // continues real process each cycle
                continue_process(current_process, sim_time);

            }
            // updates ready queue
            ready_queue = allocate_memory(memory, holes, input_queue, ready_queue, mem_strategy, sim_time,
                                          (int (*)(void *, process_t *)) insert_process);

            // starts new process
            if (new_process || (!is_empty_heap(ready_queue) && no_process_running)) {
                current_process = run_next_process(ready_queue, sim_time, (process_t *(*)(void *)) extract_min,
                                                   (int (*)(void *)) is_empty_heap);
                no_process_running = (current_process == NULL) ? 1 : 0;
            }


            // round robin algorithm - uses linked list as ready queue
        } else if (strcmp(scheduler, "RR") == 0) {

            // for first cycle
            if (num_cycles == 0) {

                update_input(input_queue, processes, sim_time);
                ready_queue = create_empty_list();
                ready_queue = allocate_memory(memory, holes, input_queue, ready_queue, mem_strategy, sim_time,
                                              (int (*)(void *, process_t *)) enqueue);
                current_process = run_next_process(ready_queue, sim_time, (process_t *(*)(void *)) dequeue,
                                                   (int (*)(void *)) is_empty_list);

                sim_time += quantum;
                continue;
            }

            // updates service time of current process and finishes process if finished
            if ((new_process = update_time(quantum, current_process))) {

                processes_remaining = get_list_size(input_queue) + get_list_size(ready_queue);

                finish_process(current_process, finished_queue, memory, holes, processes_remaining,
                               sim_time, mem_strategy);

                if (get_list_size(finished_queue) == num_processes) {
                    // all processes completed
                    break;
                }

            }

            // updates input queue
            update_input(input_queue, processes, sim_time);
            // updates ready queue
            ready_queue = allocate_memory(memory, holes, input_queue, ready_queue, mem_strategy, sim_time,
                                          (int (*)(void *, process_t *)) enqueue);

            // suspend processes and decide which to run
            if (new_process) {
                // runs a new process
                current_process = run_next_process(ready_queue, sim_time, (process_t *(*)(void *)) dequeue,
                                                   (int (*)(void *)) is_empty_list);
            } else if (!is_empty_list(ready_queue)) {
                // suspends process and runs next in queue
                enqueue(ready_queue, current_process);
                suspend_process(current_process, sim_time);
                current_process = run_next_process(ready_queue, sim_time, (process_t *(*)(void *)) dequeue,
                                                   (int (*)(void *)) is_empty_list);
            } else {
                // continues same process
                continue_process(current_process, sim_time);
            }


        }

        // update simulation time
        sim_time += quantum;

    }

    print_statistics(finished_queue, sim_time);


    // frees process and memory queues
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

/**
 * Updates the input queue as processes arrive
 *
 * @param input Queue containing unallocated processes
 * @param processes Queue containing all un-arrived processes
 * @param sim_time Current simulation time
 * @return Updated input queue
 */
list_t *update_input(list_t *input, list_t *processes, int sim_time) {

    if (get_head(processes) == NULL) {
        return NULL;
    }
    // adds processes into input queue when they arrive
    while ((int) get_value(get_data(get_head(processes)), ARRIVAL_TIME) <= sim_time) {

        enqueue(input, dequeue(processes));

        if (is_empty_list(processes)) {
            break;
        }

    }

    return input;

}

/**
 * Suspends a real process
 *
 * @param process Process to be suspended
 * @param sim_time Current simulation time
 */
void suspend_process(process_t *process, int sim_time) {

    int w_status, w;
    // sends sim time to process
    send_bytes(process, sim_time);

    // suspends process
    kill(get_value(process, PID), SIGTSTP);

    // waits for process to suspend
    do {
        w = waitpid(get_value(process, PID), &w_status, WUNTRACED);
        if (w == -1) {
            perror("waitpid");
            exit(EXIT_FAILURE);
        }

    } while (!WIFSTOPPED(w_status));

}

/**
 * Continues a process after it has been suspended or at the start of a new cycle
 * @param process Process to be continued
 * @param sim_time Current simulation time
 */
void continue_process(process_t *process, int sim_time) {

    // sends bytes to process, continues it and verifies sent bytes
    int8_t test_byte = send_bytes(process, sim_time);
    kill(get_value(process, PID), SIGCONT);
    read_and_verify(process, test_byte);

}

/**
 * Terminates a process that has completed
 *
 * @param process Process to be terminated
 * @param finished List of finished processes
 * @param memory Memory management list
 * @param holes List of holes
 * @param proc_remaining Number of processes remaining that have arrived
 * @param sim_time Current simulation time
 * @param mem_strategy Memory strategy
 */
void finish_process(process_t *process, list_t *finished, list_t *memory, list_t *holes, int proc_remaining, int sim_time,
                    char *mem_strategy) {

    char sha256[HASH_SIZE];

    set_state(process, FINISHED);
    enqueue(finished, process);
    printf("%d,FINISHED,process_name=%s,proc_remaining=%d\n", sim_time, get_name(process), proc_remaining);

    // terminate process
    send_bytes(process, sim_time);
    kill(get_value(process, PID), SIGTERM);
    // reads in hash value from process
    read(get_fd_in(process)[READ], sha256, HASH_SIZE);

    printf("%d,FINISHED-PROCESS,process_name=%s,sha=", sim_time, get_name(process));
    for (int i = 0; i < HASH_SIZE; i++) {
        printf("%c", sha256[i]);
    }
    printf("\n");

    set_value(process, sim_time, FINISH_TIME);
    update_stats(process);
    deallocate_memory(process, memory, holes, mem_strategy);

}

/**
 * Runs the next process that is in the ready queue
 *
 * @param ready Queue containing processes that have been allocated memory
 * @param sim_time Current simulation time
 * @param extract Function that takes next process from ready queue
 * @param is_empty Function that checks if ready queue is empty
 * @return Process that will be ran
 */
process_t *run_next_process(void *ready, int sim_time, process_t *(*extract)(void *), int (*is_empty)(void *)) {

    if (is_empty(ready)) {
        return NULL;
    }
    process_t *current_process = extract(ready);

    // creates new process if not already created otherwise continues it
    if (get_value(current_process, PID) == 0) {
        start_real_process(current_process, sim_time);
    } else {
        continue_process(current_process, sim_time);
    }


    set_state(current_process, RUNNING);
    printf("%d,RUNNING,process_name=%s,remaining_time=%d\n", sim_time, get_name(current_process),
           (int) get_value(current_process, SERVICE_TIME_LEFT));

    return current_process;

}

/**
 * Starts a real process from the process executable
 *
 * @param process Process to be started
 * @param sim_time Current simulation time
 */
void start_real_process(process_t *process, int sim_time) {

    int fd_out[2], fd_in[2];
    pid_t child_pid;

    // creates pipes for reading and writing to process from parent
    if (pipe(fd_out) == -1 || pipe(fd_in) == -1) {
        perror("pipe");
        exit(EXIT_FAILURE);
    }

    // forks new process
    if ((child_pid = fork()) == -1) {
        perror("fork");
        exit(1);
    }

    if (child_pid == 0) {
        // child process

        close(fd_out[WRITE]);
        close(fd_in[READ]);

        // assigns file descriptors to stdin and stdout
        dup2(fd_out[READ], STDIN_FILENO);
        dup2(fd_in[WRITE], STDOUT_FILENO);

        // replace the child process with process programs image
        char *args[] = {"process", get_name(process), NULL};
        execv(args[0], args);

        // error
        exit(EXIT_FAILURE);

    } else {
        // parent process

        close(fd_out[READ]);
        close(fd_in[WRITE]);
        set_fds(process, fd_in, fd_out);

        // sends sim time to child and verifies it
        uint8_t test_byte = send_bytes(process, sim_time);
        read_and_verify(process, test_byte);

        set_value(process, child_pid, PID);

    }

}

/**
 * Reads a byte in from the process and verifies it is the same as the last byte that was sent
 *
 * @param process Current process
 * @param test_byte Last byte written
 */
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

/**
 * Converts number to 4 bytes and sends to process
 *
 * @param process Process
 * @param num Number to be sent
 * @return Last bytes sent
 */
uint8_t send_bytes(process_t *process, int num) {

    // array of 4 8bit integers
    uint8_t time_bytes[4];
    // separates the num into 4 bytes by assigning it to the first element of array as a 32bit integer (carries over to other 3 bytes)
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

/**
 * Prints the statistics of the completed processes
 *
 * @param finished List of completed processes
 * @param makespan Final simulation time
 */
void print_statistics(list_t *finished, int makespan) {

    int avg_turnaround;
    double avg_overhead, max_overhead;
    avg_turnaround = ceil(mean(finished, TURNAROUND_TIME));
    // rounded to remove effects of floating point arithmetic
    avg_overhead = round(mean(finished, OVERHEAD) * 100) / 100;
    max_overhead = round(max(finished, OVERHEAD) * 100) / 100;

    printf("Turnaround time %d\n", avg_turnaround);
    printf("Time overhead %.2lf %.2lf\n", max_overhead, avg_overhead);
    printf("Makespan %d\n", makespan);

}

/**
 * Finds average of all the processes by given field
 *
 * @param list List of processes
 * @param field Field to calculate average of
 * @return Average of the field
 */
double mean(list_t *list, enum value field) {

    double sum = 0;
    node_t *curr = get_head(list);
    process_t *curr_proc;

    // sum of all values
    while (curr != NULL) {

        curr_proc = (process_t *) get_data(curr);
        sum += get_value(curr_proc, field);

        curr = get_next(curr);
    }

    return (double) sum / get_list_size(list);

}

/**
 * Finds maximum of all the processes by given field
 *
 * @param list List of processes
 * @param field Field to find max of
 * @return Maximum value
 */
double max(list_t *list, enum value field) {

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
