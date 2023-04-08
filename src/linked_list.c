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

struct queue {
    node_t *head;
    node_t *tail;
    int num_items;
};

queue_t *create_empty_queue() {

    queue_t *queue;
    queue = (queue_t *) malloc(sizeof(*queue));
    assert(queue);

    queue->head = NULL;
    queue->tail = NULL;
    queue->num_items = 0;

    return queue;

}


int enqueue(queue_t *queue, process_t *process) {
    // no more processes left
    if (process == NULL) {
        return 0;
    }

    node_t *new;

    new = (node_t *) malloc(sizeof(*new));
    assert(new);
    new->process = process;
    new->next = NULL;

    if (queue->head == NULL) {
        queue->head = new;
        queue->tail = new;
        (queue->num_items)++;
        return 1;
    }

    // tail extended by 1
    queue->tail->next = new;

    // new tail updated
    queue->tail = new;

    (queue->num_items)++;



    // success
    return 1;
}

process_t *dequeue(queue_t *queue) {

    node_t *head = queue->head;
    queue->head = head->next;
    (queue->num_items)--;

    return head->process;
}

void free_processes(queue_t *queue) {

    node_t *curr, *prev;

    curr = queue->head;

    while (curr != NULL) {
        prev = curr;
        curr = curr->next;
        free_process(prev->process);
        free(prev);
    }

    free(queue);
}

