/*
 * process_scheduling.c - Contains function for the process scheduling algorithms
 * Author: Tristan Thomas
 * Date: 6-4-2023
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>

#include "process_scheduling.h"
#include "linked_list.h"

enum state {
    IDLE, READY, RUNNING, FINISHED
};

queue_t *update_input(queue_t *input, queue_t *processes, int sim_time) {
    if (get_head(processes) == NULL) {
        return NULL;
    }
    while(get_value(get_process(get_head(processes)), 'a') <= sim_time) {
        enqueue(input, dequeue(processes));
        if (is_empty(processes)) {
            break;
        }
    }

    return input;

}


int update_time(int quantum, process_t *process) {
    if (get_value(process, 's') - quantum <= 0) {
        set_value(process, 0, 's');
        set_state(process, FINISHED);
        return 1;
    } else {
        set_value(process, get_value(process, 's') - quantum, 's');
        return 0;
    }

}
