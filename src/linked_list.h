/*
 * linked_list.h - Contains the doubly linked list data structure and functions declarations
 * Author: Tristan Thomas
 * Date: 7-4-2023
 */

#ifndef LINKED_LIST_H
#define LINKED_LIST_H

typedef struct node node_t;
typedef struct list list_t;


/**
 * Creates empty linked list
 *
 * @return Empty linked list
 */
list_t *create_empty_list();

/**
 * Inserts a data element at the end of a linked list
 *
 * @param list Linked list to be inserted into
 * @param data Data element
 * @return 1 if successful, if not
 */
int enqueue(list_t *list, void *data);

/**
 * Removes the head of a linked list
 *
 * @param list Linked list to be dequeued
 * @return Data element stored in head of linked list
 */
void *dequeue(list_t *list);

/**
 * Frees a linked list and its data
 *
 * @param list Linked list to be freed
 * @param free_data Function for freeing data
 */
void free_list(list_t *list, void (*free_data)(void *));

/**
 * Checks if list is empty
 *
 * @param list List to be checked
 * @return 1 if empty, 0 if not
 */
int is_empty_list(list_t *list);

/**
 * Gets the head node of a given linked list
 *
 * @param list Specified list
 * @return Head node
 */
node_t *get_head(list_t *list);

/**
 * Gets the data of a given linked list
 *
 * @param list Specified list
 * @return Data of node
 */
void *get_data(node_t *node);

/**
 * Gets the next node of a given linked list
 *
 * @param list Specified list
 * @return Next node
 */
node_t *get_next(node_t *node);

/**
 * Gets the previous node of a given linked list
 *
 * @param list Specified list
 * @return Previous node
 */
node_t *get_prev(node_t *node);

/**
 * Gets the size of a given linked list
 *
 * @param list Specified list
 * @return Number of items
 */
int get_list_size(list_t *list);

/**
 * Creates a node to be inserted
 *
 * @param data Data element
 * @param next Node to the right (towards tail)
 * @param prev Node to the left (towards head)
 * @return Newly created node
 */
node_t *create_node(void *data, node_t *next, node_t *prev);

/**
 * Inserts node into linked list in sorted order
 *
 * @param list List to be inserted into
 * @param block_node Node to be inserted
 * @param compare Comparison function
 * @param get_sort_value Function that returns value that determines sort order
 */
void insert_node_sorted(list_t *list, node_t *node, int (*compare)(void *, void *), void *(*get_sort_value)(void *));

/**
 * Deletes node in a linked list
 *
 * @param list Linked list to be deleted from
 * @param node Node to be deleted
 */
void delete_node(list_t *list, node_t *node);

/**
 * Inserts a node containing data between two given nodes
 *
 * @param list List to be inserted into
 * @param data Data to be contained in new node
 * @param prev Previous node
 * @param next Next node
 * @return
 */
node_t *insert_node(list_t* list, void *data, node_t *prev, node_t *next);

/**
 * Deletes node in a linked list by its data
 *
 * @param list Linked list to be deleted from
 * @param data Data of node to be deleted
 * @param free_data Freeing function for data
 */
void delete_node_by_data(list_t *list, void *data, void (*free_data)(void *));

/**
 * Frees a single node
 *
 * @param node Node to be freed
 * @param free_data Function to free data
 */
void free_node(node_t *node, void (*free_data)(void *));

/**
 * Function that does nothing (used for free_list when data doesnt want to be freed)
 *
 * @param blank Nothing
 */
void blank(void *blank);



#endif
