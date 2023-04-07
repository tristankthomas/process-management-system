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
typedef struct processes processes_t;

process_t *read_process(FILE **file);
processes_t *load_processes(processes_t *processes, FILE **file);
void free_process(process_t *process);

#endif
