/*
 * linked_list.c - Contains the linked list data structure and functions to initially store processes
 * Author: Tristan Thomas
 * Date: 7-4-2023
 */

#include <stdio.h>
#include <stdlib.h>
#include <assert.h>

#include "linked_list.h"
#include "process_data.h"


struct node {
    process_t *process;
    node_t *next;
};

struct processes {
    node_t *head;
    int num_items;
};

processes_t *create_empty_list() {

    processes_t *list;
    list = (processes_t *) malloc(sizeof(*list));
    assert(list);

    list->head = NULL;
    list->num_items = 0;

    return list;

}


int insert_process(processes_t *processes, process_t *process) {
    // no more processes left
    if (process == NULL) {
        return 0;
    }

    node_t *new;

    new = (node_t *) malloc(sizeof(*new));
    assert(new);
    new->process = process;
    new->next = NULL;

    if (processes->head == NULL) {
        processes->head = new;
        (processes->num_items)++;
        return 1;
    }

    node_t *current = processes->head;
    while (current->next != NULL) {
        current = current->next;
    }
    current->next = new;
    (processes->num_items)++;

    // success
    return 1;
}

process_t *pop_head(processes_t *processes) {
    node_t *head = processes->head;
    processes->head = head->next;
    (processes->num_items)--;
    return head->process;
}

void free_processes(processes_t *processes) {

    node_t *curr, *prev;

    curr = processes->head;

    while (curr != NULL) {
        prev = curr;
        curr = curr->next;
        free_process(prev->process);
        free(prev);
    }

    free(processes);
}

