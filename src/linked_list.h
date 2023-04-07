/*
 * linked_list.h - Contains the linked list data structure and functions declarations
 * Author: Tristan Thomas
 * Date: 7-4-2023
 */

#ifndef LINKED_LIST_H
#define LINKED_LIST_H

#include "process_data.h"

typedef struct node node_t;
typedef struct processes processes_t;
// to remove the circular dependency
typedef struct process process_t;

processes_t *create_empty_list();
int insert_process(processes_t *processes, process_t *process);
process_t *pop_head(processes_t *processes);
void free_processes(processes_t *processes);

#endif
