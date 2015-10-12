/*
 * tree.h
 *
 *  Created on: Sep 27, 2015
 *      Author: rob
 */
#include "board.h"
#include "linked_list.h"

#ifndef TREE_H_
#define TREE_H_

typedef struct tree {
	struct list_node* root;
	struct list* search_order;
} tree;

/* Initialize functions */
tree* create_tree();
void delete_tree(tree* tree);

/* Setters */
void set_root(tree* tree, struct board* board);

/* Game tree functions */
void generate_permutations(struct list_node** node, board* original, int nth_perm, int player);
void delete_permutations(struct tree** game_tree, board** b);

/* Minimax functions */
void max_value(struct list_node** parent, int* alpha, int* beta);
void max_decision(struct list_node** parent);
void max(int* best, struct list_node** action, struct list_node** parent);

void min_value(struct list_node** parent, int* alpha, int* beta);
void min_decision(struct list_node** parent);
void min(int* best, struct list_node** action, struct list_node** parent);

/* Minimax utility functions */
void best_horizontal_max(struct list_node** parent);
void best_vertical_max(struct list_node** parent);
void get_best_max(struct list_node** parent);

void best_horizontal_min(struct list_node** parent);
void best_vertical_min(struct list_node** parent);
void get_best_min(struct list_node** parent);


#endif /* TREE_H_ */
