/*
 * linked_list.h - Contains the linked list data structure and functions declarations
 * Author: Tristan Thomas
 * Date: 7-4-2023
 */

#ifndef LINKED_LIST_H
#define LINKED_LIST_H

#include "process_data.h"

typedef struct list_node list_node_t;
typedef struct list list_t;
// to remove the circular dependency
typedef struct process process_t;

list_t *create_empty_list();
int enqueue(list_t *list, void *data);
void *dequeue(list_t *list);
void free_list(list_t *list, void (*free_data)(void *));
int is_empty_list(list_t *list);
list_node_t *get_head(list_t *list);
list_node_t *get_tail(list_t *list);
void set_head(list_t *list, list_node_t *head);
void set_tail(list_t *list, list_node_t *tail);
void set_next(list_node_t *node, list_node_t *next);
void *get_data(list_node_t *node);
list_node_t *get_next(list_node_t *node);
list_node_t *get_prev(list_node_t *node);
int get_num_items(list_t *list);
void set_num_items(list_t *list, int num_items);
void set_prev(list_node_t *node, list_node_t *prev);
int get_list_size(list_t *list);
list_node_t *create_list_node(void *data, list_node_t *next, list_node_t *prev);
void insert_node_sorted(list_t *list, list_node_t *node, int (*compare)(void *, void *), void *(*get_sort_value)(void *));
void delete_node(list_t *list, list_node_t *node);
void insert_list_node(list_t* list, void *data, list_node_t *prev, list_node_t *next);


#endif
