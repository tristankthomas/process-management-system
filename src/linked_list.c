/*
 * linked_list.c - Contains the linked list data structure and functions to initially store processes
 * Author: Tristan Thomas
 * Date: 7-4-2023
 */

#include <stdio.h>
#include <stdlib.h>
#include <assert.h>

#include "linked_list.h"


struct node {
    void *data;
    node_t *next;
    node_t *prev;
};

struct list {
    node_t *head;
    node_t *tail;
    int num_items;
};

list_t *create_empty_list() {

    list_t *list;
    list = (list_t *) malloc(sizeof(*list));
    assert(list);

    list->head = NULL;
    list->tail = NULL;
    list->num_items = 0;

    return list;

}

/**
 *
 * @param list
 * @param data
 * @return
 */
int enqueue(list_t *list, void *data) {

    if (data == NULL) {
        return 0;
    }

    node_t *new = create_node(data, NULL, list->tail);

    if (list->head == NULL) {
        list->head = new;
        list->tail = new;
        (list->num_items)++;
        return 1;
    }

    // tail extended by 1
    list->tail->next = new;

    // new tail updated
    list->tail = new;

    list->num_items++;



    // success
    return 1;
}

/**
 *
 * @param list
 * @return
 */
void *dequeue(list_t *list) {
    if (list->head == NULL) {
        return NULL;
    }
    node_t *head = list->head;
    list->head = head->next;
    if (list->head == NULL) {
        list->tail = NULL;
    }
    list->num_items--;
    void *data = head->data;

    free(head);
    head = NULL;

    return data;
}

node_t *create_node(void *data, node_t *next, node_t *prev) {

    node_t *new_node = (node_t *) malloc(sizeof(*new_node));
    assert(new_node);
    new_node->data = data;
    new_node->next = next;
    new_node->prev = prev;

    return new_node;
}

void insert_node_sorted(list_t *list, node_t *block_node, int (*compare)(void *, void *), void *(*get_sort_value)(void *)) {
    // current and prev point to nodes for 'list' that store a node as data
    node_t *head = list->head;
    node_t *curr = head;
    node_t *prev = NULL;
    void *value = get_sort_value(get_data(block_node));

    while (curr != NULL && (compare(get_sort_value(get_data(get_data(curr))), value) < 0)) { // curr->value < value (as soon as block size is smaller insert in that pos)
        prev = curr;
        curr = curr->next;
    }

    node_t *hole_node = create_node(block_node, curr, prev);

    // don't really want to change 'node' values as I want to store 'node' as data for a node in list
    if (is_empty_list(list)) {
        list->head = hole_node;
        list->tail = hole_node;
    } else if (prev == NULL) {
        // Insert at beginning (list doesn't have to be empty but item is the smallest so no prev)
        //already done this (curr = head in this case)
        //hole_node->next = head;
        list->head = hole_node;
        curr->prev = hole_node;
    } else if (curr == NULL) {
        // Insert at tail
        list->tail = hole_node;
        prev->next = hole_node;
    } else {
        // Insert between previous and current
        prev->next = hole_node;
        //already done this
        //hole_node->next = curr;
        curr->prev = hole_node;
    }
    list->num_items++;
}

void free_list(list_t *list, void (*free_data)(void *)) {

    node_t *curr, *prev;

    curr = list->head;

    while (curr != NULL) {
        prev = curr;
        curr = curr->next;
        free_data(prev->data);
        prev->data = NULL;
        free(prev);
        prev = NULL;
    }

    free(list);
    list = NULL;
}

void delete_node(list_t *list, node_t *node) {
    if (list->head == NULL || list->tail == NULL || node == NULL) {
        return;
    }

    if (list->head == node) {
        // if nodes at head
        list->head = node->next;
    }

    if (list->tail == node) {
        // if nodes at tail
        list->tail = node->prev;
    }

    if (node->next != NULL) {
        // removing the link
        node->next->prev = node->prev;
    }

    if (node->prev != NULL) {
        node->prev->next = node->next;
    }

    list->num_items--;

    // don't need to free as keeping the node still

}

void blank(void *blank) {
    return;
}

void delete_node_by_data(list_t *list, void *data, void (*free_data)(void *)) {
    node_t *curr = get_head(list);

    while (get_data(curr) != data) {

        curr = get_next(curr);

    }
    delete_node(list, curr);
    free_node(get_data(curr), free_data);
    free(curr);
    curr = NULL;
}

void free_node(node_t *node, void (*free_data)(void *)) {
    free_data(node->data);
    free(node);
    node = NULL;
}

node_t *insert_node(list_t* list, void *data, node_t *prev, node_t *next) {
    node_t *new = create_node(data, next, prev);

    if (prev != NULL) {
        prev->next = new;
    } else {
        list->head = new;
    }
    if (next != NULL) {
        next->prev = new;
    } else {
        list->tail = new;
    }

    list->num_items++;

    return new;
}

int is_empty_list(list_t *list) {
    return (list->num_items == 0);
}

node_t *get_head(list_t *list) {
    return list->head;
}

node_t *get_tail(list_t *list) {
    return list->tail;
}

void set_head(list_t *list, node_t *head) {
    list->head = head;
}

void set_tail(list_t *list, node_t *tail) {
    list->tail = tail;
}

void *get_data(node_t *node) {
    return node->data;
}

node_t *get_next(node_t *node) {
    return node->next;
}

node_t *get_prev(node_t *node) {
    return node->prev;
}

int get_num_items(list_t *list) {
    return list->num_items;
}

void set_prev(node_t *node, node_t *prev) {
    node->prev = prev;
}

void set_next(node_t *node, node_t *next) {
    node->next = node;
}

void set_num_items(list_t *list, int num_items) {
    list->num_items = num_items;
}

int get_list_size(list_t *list) {
    return list->num_items;
}