/*
 * process_data.h - Contains the process data structure declarations
 * Author: Tristan Thomas
 * Date: 6-4-2023
 */

#ifndef PROCESS_DATA_H
#define PROCESS_DATA_H

#include "linked_list.h"

#define MAX_NAME_LEN 8

typedef enum state state_t;
typedef struct process process_t;
typedef struct node node_t;
typedef struct queue queue_t;

process_t *read_process(FILE **file);
queue_t *load_processes(queue_t *processes, FILE **file);
void free_process(process_t *process);
int get_value(process_t *process, char field);
process_t *get_process(node_t *node);
int cmp_service_time(process_t *p1, process_t *p2);
void set_state(process_t *process, state_t state);

#endif
