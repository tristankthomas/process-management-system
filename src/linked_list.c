/*
 * linked_list.c - Contains the doubly linked list data structure and functions
 * Author: Tristan Thomas
 * Date: 7-4-2023
 */

#include <stdio.h>
#include <stdlib.h>
#include <assert.h>

#include "linked_list.h"

/* Definitions of double linked list node and main list */
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


/**
 * Creates empty linked list
 *
 * @return Empty linked list
 */
list_t *create_empty_list() {

    list_t *list = malloc(sizeof(*list));
    assert(list);

    list->head = NULL;
    list->tail = NULL;
    list->num_items = 0;

    return list;

}

/**
 * Inserts a data element at the end of a linked list
 *
 * @param list Linked list to be inserted into
 * @param data Data element
 * @return 1 if successful, if not
 */
int enqueue(list_t *list, void *data) {

    if (!data) {
        return 0;
    }

    node_t *new = create_node(data, NULL, list->tail);

    // Empty linked list
    if (!list->head) {
        list->head = new;
        list->tail = new;
        list->num_items++;
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
 * Removes the head of a linked list
 *
 * @param list Linked list to be dequeued
 * @return Data element stored in head of linked list
 */
void *dequeue(list_t *list) {
    if (!list->head) {
        return NULL;
    }
    node_t *head = list->head;
    list->head = head->next;
    if (!list->head) {
        list->tail = NULL;
    }
    list->num_items--;
    void *data = head->data;

    free(head);
    head = NULL;

    return data;
}

/**
 * Creates a node to be inserted
 *
 * @param data Data element
 * @param next Node to the right (towards tail)
 * @param prev Node to the left (towards head)
 * @return Newly created node
 */
node_t *create_node(void *data, node_t *next, node_t *prev) {

    node_t *new_node = malloc(sizeof(*new_node));
    assert(new_node);
    new_node->data = data;
    new_node->next = next;
    new_node->prev = prev;

    return new_node;
}

/**
 * Inserts node into linked list in sorted order
 *
 * @param list List to be inserted into
 * @param block_node Node to be inserted
 * @param compare Comparison function
 * @param get_sort_value Function that returns value that determines sort order
 */
void insert_node_sorted(list_t *list, node_t *block_node, int (*compare)(void *, void *), void *(*get_sort_value)(void *)) {

    node_t *head = list->head;
    node_t *curr = head;
    node_t *prev = NULL;
    void *value = get_sort_value(get_data(block_node));

    // as soon as block size is smaller insert in that position
    while (curr && (compare(get_sort_value(get_data(get_data(curr))), value) < 0)) {
        prev = curr;
        curr = curr->next;
    }

    // creates new node
    node_t *hole_node = create_node(block_node, curr, prev);


    if (is_empty_list(list)) {
        list->head = hole_node;
        list->tail = hole_node;
    } else if (!prev) {
        // Insert at beginning
        list->head = hole_node;
        curr->prev = hole_node;
    } else if (!curr) {
        // Insert at tail
        list->tail = hole_node;
        prev->next = hole_node;
    } else {
        // Insert between previous and current
        prev->next = hole_node;
        curr->prev = hole_node;
    }

    list->num_items++;

}

/**
 * Frees a linked list and its data
 *
 * @param list Linked list to be freed
 * @param free_data Function for freeing data
 */
void free_list(list_t *list, void (*free_data)(void *)) {

    node_t *curr, *prev;

    curr = list->head;

    while (curr) {
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

/**
 * Function that does nothing (used for free_list when data doesnt want to be freed)
 *
 * @param blank Nothing
 */
void blank(void *blank) {

    return;
}

/**
 * Deletes node in a linked list
 *
 * @param list Linked list to be deleted from
 * @param node Node to be deleted
 */
void delete_node(list_t *list, node_t *node) {

    if (!list->head || !list->tail || !node) {
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

    if (node->next) {
        // removing the link
        node->next->prev = node->prev;
    }

    if (node->prev) {
        node->prev->next = node->next;
    }

    list->num_items--;

}

/**
 * Deletes node in a linked list by its data
 *
 * @param list Linked list to be deleted from
 * @param data Data of node to be deleted
 * @param free_data Freeing function for data
 */
void delete_node_by_data(list_t *list, void *data, void (*free_data)(void *)) {

    node_t *curr = get_head(list);

    // finds node with data
    while (get_data(curr) != data) {
        curr = get_next(curr);
    }

    delete_node(list, curr);
    free_node(get_data(curr), free_data);
    free(curr);
    curr = NULL;

}

/**
 * Frees a single node
 * @param node Node to be freed
 * @param free_data Function to free data
 */
void free_node(node_t *node, void (*free_data)(void *)) {

    free_data(node->data);
    free(node);
    node = NULL;
}

/**
 * Inserts a node containing data between two given nodes
 *
 * @param list List to be inserted into
 * @param data Data to be contained in new node
 * @param prev Previous node
 * @param next Next node
 * @return
 */
node_t *insert_node(list_t* list, void *data, node_t *prev, node_t *next) {

    node_t *new = create_node(data, next, prev);

    if (prev) {
        prev->next = new;
    } else {
        list->head = new;
    }
    if (next) {
        next->prev = new;
    } else {
        list->tail = new;
    }

    list->num_items++;

    return new;
}

/**
 * Checks if list is empty
 *
 * @param list List to be checked
 * @return 1 if empty, 0 if not
 */
int is_empty_list(list_t *list) {
    return (list->num_items == 0);
}

/**
 * Gets the head node of a given linked list
 *
 * @param list Specified list
 * @return Head node
 */
node_t *get_head(list_t *list) {

    return list->head;
}

/**
 * Gets the data of a given linked list
 *
 * @param list Specified list
 * @return Data of node
 */
void *get_data(node_t *node) {

    return node->data;
}

/**
 * Gets the next node of a given linked list
 *
 * @param list Specified list
 * @return Next node
 */
node_t *get_next(node_t *node) {

    return node->next;
}

/**
 * Gets the previous node of a given linked list
 *
 * @param list Specified list
 * @return Previous node
 */
node_t *get_prev(node_t *node) {

    return node->prev;
}

/**
 * Gets the size of a given linked list
 *
 * @param list Specified list
 * @return Number of items
 */
int get_list_size(list_t *list) {

    return list->num_items;
}
