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
    void *data;
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


int enqueue(queue_t *queue, void *data) {

    if (data == NULL) {
        return 0;
    }

    node_t *new;

    new = (node_t *) malloc(sizeof(*new));
    assert(new);
    new->data = data;
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

void *dequeue(queue_t *queue) {
    if (queue->head == NULL) {
        return NULL;
    }
    node_t *head = queue->head;
    queue->head = head->next;
    if (queue->head == NULL) {
        queue->tail = NULL;
    }
    (queue->num_items)--;


    return head->data;
}

void free_list(queue_t *queue, void (*free_data)(void *)) {

    node_t *curr, *prev;

    curr = queue->head;

    while (curr != NULL) {
        prev = curr;
        curr = curr->next;
        free_data(prev->data);
        free(prev);
    }

    free(queue);
}

int is_empty_queue(queue_t *queue) {
    return (queue->num_items == 0);
}

node_t *get_head(queue_t *queue) {
    return queue->head;
}

void *get_data(node_t *node) {
    return node->data;
}

node_t *get_next(node_t *node) {
    return node->next;
}

int get_queue_size(queue_t *queue) {
    return queue->num_items;
}

