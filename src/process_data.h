/*
 * process_data.h - Contains the process data structure declarations
 * Author: Tristan Thomas
 * Date: 6-4-2023
 */

#ifndef PROCESS_DATA_H
#define PROCESS_DATA_H

#include "linked_list.h"
#include "memory_allocation.h"

#define MAX_NAME_LEN 8



typedef enum state state_t;
typedef struct process process_t;
typedef struct node node_t;
typedef struct list list_t;
typedef struct block block_t;

process_t *read_process(FILE **file);
list_t *load_processes(list_t *processes, FILE **file);
void free_process(process_t *process);
int get_value(process_t *process, char field);
process_t *get_process(node_t *node);
int cmp_service_time(process_t *p1, process_t *p2);
void set_state(process_t *process, state_t state);
void set_value(process_t *process, int value, char field);
char *get_name(process_t *process);
void set_block(process_t *process, block_t *block);
block_t *get_block(process_t *process);

#endif
