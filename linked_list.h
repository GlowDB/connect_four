/*
 * linked_list.h
 *
 *  Created on: Sep 27, 2015
 *      Author: rob
 */
#ifndef LINKED_LIST_H_
#define LINKED_LIST_H_
#include "board.h"
#include "tree.h"

struct list_node;
struct linked_list;

typedef struct list_node {
  struct board* value;
  struct node* next;
  struct list* children;
} node;

typedef struct list {
  node* head;
  node* tail;
  int size;
} list;

/*
 * Initialization functions
 */
struct list* create_list();
void delete_list(struct list* list);
node* create_node(struct board* b);
void delete_node(node* node);
void error(char* msg);

/*
 * Linked list functions
 */
void push(struct list** list, node** node);
struct list_node* pop(struct list* list);
node* get(struct list* list);
int get_size(struct list* list);
void print_list(struct list* list);
void print_node(node* node);

/*
 * Tree functions
 */
void add_child(node** parent, node** child);
struct list* get_children(node** parent);
void print_children(node** node);
#endif /* LINKED_LIST_H_ */
