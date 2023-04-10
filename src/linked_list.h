/*
 * linked_list.h - Contains the linked list data structure and functions declarations
 * Author: Tristan Thomas
 * Date: 7-4-2023
 */

#ifndef LINKED_LIST_H
#define LINKED_LIST_H

#include "process_data.h"

typedef struct node node_t;
typedef struct queue queue_t;
// to remove the circular dependency
typedef struct process process_t;

queue_t *create_empty_queue();
int enqueue(queue_t *queue, void *data);
void *dequeue(queue_t *queue);
void free_list(queue_t *queue, void (*free_data)(void *));
int is_empty_queue(queue_t *queue);
node_t *get_head(queue_t *queue);
void *get_data(node_t *node);
node_t *get_next(node_t *node);
int get_queue_size(queue_t *queue);


#endif
