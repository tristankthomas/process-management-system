/*
 * process_data.h - Contains the process data structure declarations
 * Author: Tristan Thomas
 * Date: 6-4-2023
 */

#ifndef PROCESS_DATA_H
#define PROCESS_DATA_H

#include <stdio.h>
#include <stdint.h>

#define MAX_NAME_LEN 8
#define READ 0
#define WRITE 1


typedef enum state state_t;
typedef struct process process_t;
typedef struct node node_t;
typedef struct list list_t;
typedef struct block block_t;

enum state {
    IDLE, READY, RUNNING, FINISHED
};

enum value {
    ARRIVAL_TIME, SERVICE_TIME, SERVICE_TIME_LEFT, MEMORY_REQUIREMENT, FINISH_TIME, TURNAROUND_TIME, OVERHEAD, PID
};

/**
 * Loads all processes into linked list from a file
 *
 * @param processes List of processes
 * @param file File to be read from
 * @return Updated list of processes
 */
list_t *load_processes(list_t *processes, FILE **file);

/**
 * Frees a process
 *
 * @param process Process to be freed
 */
void free_process(process_t *process);

/**
 * Gets a value from a process
 *
 * @param process Specified process
 * @param field Value type to return
 * @return Value
 */
double get_value(process_t *process, enum value field);

/**
 * Compares service time used for min heap sorting
 *
 * @param p1 Process 1
 * @param p2 Process 2
 * @return An int based on comparison property
 */
int cmp_service_time(process_t *p1, process_t *p2);

/**
 * Sets state of process
 *
 * @param process Process to be updated
 * @param state New state
 */
void set_state(process_t *process, state_t state);

/**
 * Sets a value of a process
 *
 * @param process Process to be updated
 * @param value Integer value
 * @param field Required field to update
 */
void set_value(process_t *process, uint32_t value, enum value field);

/**
 * Gets process name
 *
 * @param process Specified process
 * @return Process name
 */
char *get_name(process_t *process);

/**
 * Sets the memory block of a process (as a node of memory list)
 *
 * @param process Process to be updated
 * @param block_node Memory block node from memory
 */
void set_block_node(process_t *process, node_t *block_node);

/**
 * Gets the block node from a process
 *
 * @param process Specified process
 * @return Block node
 */
node_t *get_block_node(process_t *process);

/**
 * Updates turnaround time and overhead of a process
 *
 * @param process Process to be updated
 */
void update_stats(process_t *process);

/**
 * Updates service time of a process
 *
 * @param quantum Amount of time to be subtracted
 * @param process Process to be updated
 * @return 1 if process is finished, 0 otherwise
 */
int update_time(int quantum, process_t *process);


/**
 * Sets the file descriptor pipe pairs of a process
 *
 * @param process Specified process
 * @param fd_in File descriptor pair for reading from real process
 * @param fd_out File descriptor pair for writing to real process
 */
void set_fds(process_t *process, int fd_in[], int fd_out[]);

/**
 * Gets file descriptor pair for reading from real process
 *
 * @param process Specified process
 * @return File descriptor pair
 */
int *get_fd_in(process_t *process);

/**
 * Gets file descriptor pair for writing to real process
 *
 * @param process Specified process
 * @return File descriptor pair
 */
int *get_fd_out(process_t *process);

#endif
