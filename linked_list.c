#include <stdio.h>
#include <stdlib.h>
#include "linked_list.h"
#include "board.h"
#include "tree.h"

/**
 * Create, allocate, and return a single-linked list struct
 * @return allocated list struct
 */
struct list* create_list()
{
  struct list* l = (struct list*) malloc(sizeof(list));
  if (l == NULL) { error("Could not allocate memory for list"); }
  l -> head = NULL;
  l -> tail = NULL;
  l -> size = 0;
  return l;
}

/**
 * Deallocate memory for provided list struct and all its list-nodes
 * @param list: the list to deallocate
 */
void delete_list(struct list* list)
{
	struct list_node* current = (struct list_node*) list -> head;
	while (current != NULL) {
		struct list_node* ptr = (struct list_node*) current -> next;
		delete_node(current);
		current = ptr;
	}
	free(list);
}

/**
 * Create, allocate, and return a list_node struct
 * @param x: the value to give the list_node
 * @return allocated list_node struct
 */
node* create_node(struct board* b)
{
  node* n = (node*) malloc(sizeof(node));
  if (n == NULL) { error("Could not allocate memory for list"); }
  n -> value = b;
  n -> next = NULL;
  n -> children = create_list();
  return n;
}

/**
 * Deallocate memory for provided list_node struct
 * @param n: the list_node to deallocate
 */
void delete_node(node* n)
{
	struct list* children = (struct list*) n -> children;
	delete_list(children);
	delete_board(n->value);
	free(n);
}

/**
 * Append a list_node to the end of the list
 * @param list: the list to add the list_node to
 * @param to_add: the list_node to be appended
 */
void push(struct list** list, node** to_add)
{
  if (*list == NULL) {
  	printf("Error pushing -- null list\n");
  	exit(1);
  } else if (*to_add == NULL) {
  	printf("Error pushing -- null node\n");
  	exit(1);
  }

  int size = get_size(*list);
  if (size == 0) {
    (*list) -> head = *to_add;
    (*list) -> tail = *to_add;
    (*list) -> size = 1;
  } else {
    node* temp = (*list) -> tail;
    temp -> next = (struct node*) *to_add;
    (*list) -> tail = *to_add;
    (*list) -> size = size + 1;
  }  
}

/**
 * Remove, deallocate, and return the element stored in the list_node
 * @param list: the list to be popped from
 * @return: the value stored in the head element of the list
 */
struct list_node* pop(struct list* list)
{
  if (list == NULL) { exit(1); }
  int size = get_size(list);
  node* result;
  if (size == 0) {
    printf("Error removing from list\n");
  	exit(0);
  } else if (size == 1) {
    result = list -> head;
    list -> head = NULL;
    list -> tail = NULL;
    list -> size = 0;
  } else {
    result = list -> head;
    list -> head = (node*) result -> next;
    list -> size = size - 1;
  }
  return result;
}

/**
 * Peek operation to get the first list_node in the list
 * @param list: the list for the operation to be performed on
 * @return: the list_node stored in the head of the list
 */
node* get(struct list* list)
{
  if (list == NULL) { exit(1); }
  node* result;
  int size = get_size(list);
  if (size == 0) {
    result = NULL;
  } else {
    result = list -> head;
  }
  return result;
}

/**
 * Returns the number of list_nodes in the list
 * @param list: the list for the operation to be performed on
 * @return: the size (integer) of the list
 */
int get_size(struct list* list)
{
	/*if (list == NULL) {
		printf("Cannot return size of null list\n");
		exit(1);
	}*/
  return list -> size;
}

/**
 * Prints the elements stored in the list_nodes, from head to tail
 * @param list: the list for the operation to be performed on
 */
void print_list(struct list* list)
{
  node* ptr = get(list);
  while (ptr != NULL) {
    print_node(ptr);
    ptr = (node*) ptr -> next;
  }
}

/**
 * Prints the element stored in the list_node
 * @param node: the list_node for the operation to be performed on
 */
void print_node(node* node)
{
  //printf("%d\n", node->value);
	board* board = (struct board*) node -> value;
	print_board(board);
}

/**
 * Returns a list of child nodes stored in a list_node's children list
 * @param parent: the node from which to retrieve the children list
 * @return: a list of list_nodes
 */
struct list* get_children(node** parent)
{
  struct list* child_list = (*parent) -> children;
  return child_list;
}

/**
 * Appends the child list_node to the parent list_node's children list
 * @param parent: the parent to append the child to
 * @param child: the list_node to be appended
 */

void add_child(node** parent, node** child)
{
  struct list* list = (*parent) -> children;
  push(&list, child);
}

/**
 * Print the children of the parent node, from head to tail
 * @param parent: the parent list_node to print the children for
 */
void print_children(node** parent)
{
  struct list* child_list = (struct list*) (*parent) -> children;
  node* ptr = (node*) child_list -> head;
  while (ptr != NULL) {
    printf("Child ");
    print_node(ptr);
    ptr = (node*) ptr -> next;
  }
}

