#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <omp.h>
#include "linked_list.h"
#include "board.h"
#include "tree.h"

/**
 * This function creates and returns a tree structure.
 * @return an initialized tree
 */
tree* create_tree()
{
  tree* t = (tree*) malloc(sizeof(tree));
  t -> root = NULL;
  t -> search_order = (struct list*) create_list();
  return t;
}

/**
 * This function deletes the specified tree and deallocates its memory.
 * @param tree: the tree to be deleted
 */
void delete_tree(tree* tree)
{
	struct list_node* root = (struct list_node*) tree -> root;
	delete_node(root);
	free(tree->search_order);
	free(tree);
}

/**
 * This function sets the root for the specified tree
 * @param tree: the tree whose root is to be set
 * @param board: the game board to assign to the tree's root
 */
void set_root(tree* tree, struct board* board)
{
	struct list_node* root = (struct list_node*) tree -> root;
	if (root != NULL)
		delete_node(root);
	root = create_node(board);
  tree -> root = (struct list_node*) root;
}

/**
 * This function finds the current state of the board in the root's list of children,
 * sets the child as the new root node, and deletes the former root and its children.
 * @param game_tree: the tree struct holding the game state(s) in memory
 * @param b: the current game state
 */
void delete_permutations(tree** game_tree, board** b)
{
	delete_list((*game_tree) -> root -> children);
	(*game_tree) -> root -> children = create_list();
}

/**
 * This function generates the nth permutation of the current game state. It
 * enumerates each possible move for the parent board, appends those child
 * boards to the parent board, and recursively calls itself until the recursion
 * limit is reached.
 * @param parent: the list node to be appended to
 * @param b: the game state belonging to the parent node
 * @param nth_perm: the current permutation being generated
 * @param player: the player for whom moves are to be enumerated
 */
void generate_permutations(struct list_node** parent, board* b, int nth_perm, int player)
{
	// Check recursion depth
	if (nth_perm == 6) { return; }
	nth_perm += 1;

	// Setup loop
	int num_columns = b -> column_len;
	int i;

	// Swap players
	if (player == 1)
		player = 2;
	else
		player = 1;

	// Iterate over columns and enumerate game board
	for (i = 0; i < num_columns; i++) {
		board* permutation = copy_board(b);
		// Ensure the move is valid
		if (add_checker(permutation, i, player) == 1) {
			delete_board(permutation);
		} else {
			permutation -> move = i;
			struct list_node* child = create_node(permutation);
			add_child(parent, &child);

			if (terminal_test(permutation) > 0) {
				// fall through, don't enumerate finished board
			} else {
				generate_permutations(&child, child->value, nth_perm, player);
			}
		}
	}
}

/**
 * This function returns the maximum decision for the player.
 * It takes the root node of the game tree as an argument and calculates the
 * minimax value for each action.
 * The best score and optimal move are stored in the root node once found.
 * @param parent: memory address of the game tree root
 */
void max_decision(struct list_node** parent)
{
	int alpha = -999, beta = 999;
	struct list* actions = (struct list*) (*parent) -> children;
	struct list_node* action = (struct list_node*) actions -> head;

	int best = -999;
	while (action != NULL) {
		min_value(&action, &alpha, &beta);
		if (best < action -> value -> best_score) {
			best = action -> value -> best_score;
			(*parent) -> value -> best_score = best;
			(*parent) -> value -> move = action -> value -> move;
		}
		action = (struct list_node*) action -> next;
	}
}

/**
 * This function returns the minimum decision for the player.
 * It takes the root node of the game tree as an argument and calculates the
 * minimax value for each action.
 * The best score and optimal move are stored in the root node once found.
 * @param parent: memory address of the game tree root
 */
void min_decision(struct list_node** parent)
{
	int alpha = -999, beta = 999;
	struct list* actions = (struct list*) (*parent) -> children;
	struct list_node* action = (struct list_node*) actions -> head;

	int best = 999;
	while (action != NULL) {
		max_value(&action, &alpha, &beta);
		if (best > action -> value -> best_score) {
			best = action -> value -> best_score;
			(*parent) -> value -> best_score = best;
			(*parent) -> value -> move = action -> value -> move;
		}
		action = (struct list_node*) action -> next;
	}
}

/**
 * This function performs a depth-first search on the given list node until
 * it finds a terminal state or a nested child node with no children.
 * It attempts to find and return the maximum value of the children of the
 * parent node.
 * @param parent: the list node whose children are to be searched
 */
void min_value(struct list_node** parent, int* alpha, int* beta)
{
	if (terminal_test((*parent) -> value) > 0) {
		get_best_min(parent);
	} else if (get_size((*parent) -> children) == 0) {
		get_best_min(parent);
	} else {
		struct list* actions = (*parent) -> children;
		struct list_node* action = (struct list_node*) actions -> head;
		int best = 999;

		while (action != NULL) {
			max_value(&action, alpha, beta);
			min(&best, &action, parent);
			if (best <= *alpha) { return; }
			*beta = (*beta < best) ? *beta : best;
			action = (struct list_node*) action -> next;
		}
	}
}

/**
 * This function performs a depth-first search on the given list node until
 * it finds a terminal state or a nested child node with no children.
 * It attempts to find and return the minimum value of the children of the
 * parent node.
 * @param parent: the list node whose children are to be searched
 */
void max_value(struct list_node** parent, int* alpha, int* beta)
{
	if (terminal_test((*parent) -> value) > 0) {
		get_best_max(parent);
	} else if (get_size((*parent) -> children) == 0) {
		get_best_max(parent);
	} else {
		struct list* actions = (*parent) -> children;
		struct list_node* action = (struct list_node*) actions -> head;
		int best = -999;

		while (action != NULL) {
			min_value(&action, alpha, beta);
			max(&best, &action, parent);
			if (best >= *beta) { return; }
			*alpha = (*alpha > best) ? *alpha : best;
			action = (struct list_node*) action -> next;
		}
	}
}

/**
 * This function determines the minimum value between best and action.
 * If action's value is less than best, then best is assigned action's value
 * and the parent node's best_score field is updated to the best value.
 * @param best: the current minimum value
 * @param action: the action to be compared against best
 * @param parent: the parent of action
 */
void min(int* best, struct list_node** action, struct list_node** parent)
{
	if (*best > (*action) -> value -> best_score) {
		*best = (*action) -> value -> best_score;
		(*parent) -> value -> best_score = *best;
	}
}

/**
 * This function determines the maximum value between best and action.
 * If action's value is greater than best, then best is assigned action's value
 * and the parent node's best_score field is updated to the best value.
 * @param best: the current minimum value
 * @param action: the action to be compared against best
 * @param parent: the parent of action
 */
void max(int* best, struct list_node** action, struct list_node** parent)
{
	if (*best < (*action) -> value -> best_score) {
		*best = (*action) -> value -> best_score;
		(*parent) -> value -> best_score = *best;
	}
}

/**
 * This function attempts to find the best move for the maximum
 * player, examining the state of the game board along the horizontal,
 * vertical, and diagonal.
 * @param parent: memory address of the list_node containing the game board
 */
void get_best_max(struct list_node** parent)
{
	best_vertical_max(parent);
	best_horizontal_max(parent);
}

/**
 * This function attempts to find the best move for the minimum
 * player, examining the state of the game board along the horizontal,
 * vertical, and diagonal.
 * @param parent: memory address of the list_node containing the game board
 */
void get_best_min(struct list_node** parent)
{
	best_vertical_min(parent);
	best_horizontal_min(parent);
}

/**
 * This function attempts to find the best move on the game board
 * for the minimum player along the vertical.
 * @param parent: memory address of the list_node containing the game board
 */
void best_vertical_max(struct list_node** parent)
{
	board* b = (*parent) -> value;
	int best = -1;

	/*if (terminal_test(b) == 1) {
		best = (*parent) -> value -> best_score = 10;
	} else if (terminal_test(b) == 2) {
		best = (*parent) -> value -> best_score = -10;
	} else {*/
	int num_cols = b->column_len;
	int r = b->r;

	// Move that generated this board
	int move = b -> move;

	// End index, first non-zero index
	int end = move;
	while (end < b->size) {
		if (b->array[end] != 0) {
			break;
		} else {
			end += num_cols;
		}
	}

	// Start index
	int start;
	start = (end - r * num_cols > 0) ? end - (r-1) * num_cols : move;

	// Iterate over r squares at a time, starting at at the top of the board,
	// working down to the last non-zero square
	int i, j;
	for (i = start; i <= end; i+=num_cols) {
		int p1 = 0, p2 = 0;
		int temp = 0, invalid = 0;
		for (j = 0; j < b -> r; j++) {

			// Make sure index is within range
			if (i + j * num_cols > b -> size) {
				invalid = 1;
				break;
			} else {
				int current = b -> array[i + j * num_cols];
				if (current == 1)
					p1 += 1;
				else if (current == 2)
					p2 -= 1;
			}
		}
		// If index was invalid, don't consider for best
		if (invalid == 1) {
			// fall through
		} else {
			temp = p1 + p2;
			if (temp < best)
				best = temp;

			if (p2 == 4)
				best = -10;
			else if (p1 == 4)
				best = 10;
			else if (p2 == -3 && p1 == 0)
				best = -4;
			else if (p2 == -1 && p1 == 3)
				best = -5;
			else if (p2 == 0 && p1 == 2)
				best = -3;
			else if (p2 == -2 && p1 == 0)
				best = -3;

			if (best < (*parent) -> value -> best_score) {
				(*parent) -> value -> best_score = best;
				//printf("BEST VERTICAL MOVE %d SCORE %d\n", move, best);
				//print_board(b);
			}
		}
	}
	//}
}

/**
 * This function attempts to find the best move on the game board
 * for the maximum player along the vertical.
 * @param parent: memory address of the list_node containing the game board
 */
void best_vertical_min(struct list_node** parent)
{
	board* b = (*parent) -> value;
	int best = -1;

	/*if (terminal_test(b) == 1) {
		best = (*parent) -> value -> best_score = 10;
	} else if (terminal_test(b) == 2) {
		best = (*parent) -> value -> best_score = -10;
	} else {*/
	int num_cols = b->column_len;
	int r = b->r;

	// Move that generated this board
	int move = b -> move;

	// Find target index, first non-zero index
	int target = move;
	while (target < b->size) {
		if (b->array[target] != 0) {
			break;
		} else {
			target += num_cols;
		}
	}

	// Start index
	int start;
	start = (target - r * num_cols > 0) ? target - (r-1) * num_cols : move;

	// End index
	int end = target, counter = 0;
	while ((end + num_cols) < b -> size && counter < r) {
		end += num_cols;
		counter += 1;
	}

	// Iterate over r squares at a time, starting at at the top of the board,
	// working down to the last non-zero square
	int i, j;
	for (i = start; i <= target; i+=num_cols) {
		if (i + (r-1) * num_cols > end) {
			// fall through
		} else {
			int p1 = 0, p2 = 0;
			int temp = 0, invalid = 0;
			//printf("VERTICAL MAX COLUMN %d\n", move);
			//print_board(b);
			for (j = 0; j < b -> r; j++) {

				// Make sure index is within range
				if (i + j * num_cols > b -> size) {
					invalid = 1;
					break;
				} else {
					int current = b -> array[i + j * num_cols];
					//printf("%d", current);
					if (current == 1)
						p1 += 1;
					else if (current == 2)
						p2 -= 1;
				}
			}
			//printf("\n");
			// If index was invalid, don't consider for best
			if (invalid == 1) {
				// fall through
			} else {
				temp = p1 + p2;
				if (temp > best)
					best = temp;

				if (p1 == 4)
					best = 4;
				else if (p2 == 4)
					best = -10;
				else if (p1 == 3 && p2 == 0)
					best = 4;
				else if (p1 == 1 && p2 == -3)
					best = 5;
				else if (p1 == 2 && p2 == 0)
					best = 3;
				else if (p1 == 0 && p2 == -2)
					best = 3;

				if (best > (*parent) -> value -> best_score) {
					(*parent) -> value -> best_score = best;
					//printf("BEST VERTICAL MAX %d %d\n", best, move);
				}
			}
		}
	}
	//}
}

/**
 * This function attempts to find the best move on the game board
 * for the minimum player along the vertical.
 * @param parent: memory address of the list_node containing the game board
 */
void best_horizontal_max(struct list_node** parent)
{
	board* b = (*parent) -> value;
	int best = -1;

	/*if (terminal_test(b) == 1) {
		best = (*parent) -> value -> best_score = 10;
	} else if (terminal_test(b) == 2) {
		best = (*parent) -> value -> best_score = -10;
	} else {*/
	int num_cols = b->column_len;
	int r = b->r;

	// Move that generated this board
	int move = b -> move;

	// Find target index,
	// first non-zero index in column where checker was placed
	int target = move;
	while (target < b->size) {
		if (b->array[target] != 0) {
			break;
		} else {
			target += num_cols;
		}
	}

	// Row
	int row = target / num_cols;

	// Start index
	int start = target;
	int counter = 0;
	while ((start) / num_cols == row && counter < 4) {
		start -= 1;
		counter += 1;
	}
	start += 1;

	// End index
	int end = target;
	counter = 0;
	while ((end) / num_cols == row && counter < 4) {
		end += 1;
		counter += 1;
	}
	end -= 1;

	int i, j;
	//printf("BEST HORIZONTAL MIN COLUMN %d\n", move);
	//print_board(b);
	for (i = start; i <= end; i++) {

		// Ensure valid index
		if (i + (r-1) > end) {
			// fall through
		} else {
			int p1 = 0, p2 = 0;
			int temp, invalid = 0;
			for (j = 0; j < r; j++) {
				// Make sure row below has checker
				if (i + j + 7 < b->size && b->array[i+j+7] == 0) {
					invalid = 1;
					break;
				} else {
					int current = b -> array[i + j];
					//printf("%d", current);
					if (current == 1)
						p1 += 1;
					else if (current == 2)
						p2 -= 1;
				}
			}
			//printf("\n");
			if (invalid == 0) {
				temp = p1 + p2;
				if (temp < best)
					best = temp;

				if (p2 == 4)
					best = -10;
				else if (p1 == 4)
					best = 10;
				else if (p2 == -3 && p1 == 0)
					best = -4;
				else if (p2 == -1 && p1 == 3)
					best = -5;
				else if (p2 == -2 && p1 == 0)
					best = -3;
				else if (p2 == 0 && p1 == 2)
					best = -3;

				if (best < (*parent) -> value -> best_score) {
					(*parent) -> value -> best_score = best;
					//printf("MIN HORIZONTAL MOVE %d SCORE %d\n", move, best);
					//print_board(b);
				}
			}
		}
	}
	//}
}

/**
 * This function attempts to find the best move on the game board
 * for the maximum player along the vertical.
 * @param parent: memory address of the list_node containing the game board
 */
void best_horizontal_min(struct list_node** parent)
{
	board* b = (*parent) -> value;
	int best = -1;

	/*if (terminal_test(b) == 1) {
		best = (*parent) -> value -> best_score = 10;
	} else if (terminal_test(b) == 2) {
		best = (*parent) -> value -> best_score = -10;
	} else {*/
	int num_cols = b->column_len;
	int r = b->r;

	// Move that generated this board
	int move = b -> move;

	// Find target index, first non-zero index in column where checker was placed
	int target = move;
	while (target < b->size) {
		if (b->array[target] != 0) {
			break;
		} else {
			target += num_cols;
		}
	}

	// Row
	int row = target / num_cols;

	// Start index
	int start = target;
	int counter = 0;
	while ((start) / num_cols == row && counter < 4) {
		start -= 1;
		counter += 1;
	}
	start += 1;

	// End index
	int end = target;
	counter = 0;
	while ((end) / num_cols == row && counter < 4) {
		end += 1;
		counter += 1;
	}
	end -= 1;

	int i, j;
	//printf("BEST HORIZONTAL MAX COLUMN %d\n", move);
	//print_board(b);
	for (i = start; i <= end; i++) {

		// Ensure valid index
		if (i + (r-1) > end) {
			// fall through
		} else {
			int p1 = 0, p2 = 0;
			int temp, invalid = 0;
			for (j = 0; j < r; j++) {
				// Make sure row below has checker
				if (i + j + 7 < b->size && b->array[i+j+7] == 0) {
					invalid = 1;
					break;
				} else {
					int current = b -> array[i + j];
					//printf("%d", current);
					if (current == 1)
						p1 += 1;
					else if (current == 2)
						p2 -= 1;
				}
			}
			//printf("\n");
			if (invalid == 0) {
				temp = p1 + p2;
				if (temp > best)
					best = temp;

				if (p1 == 4)
					best = 10;
				else if (p2 == 4)
					best = -10;
				else if (p1 == 3 && p2 == 0)
					best = 4;
				else if (p1 == 1 && p2 == -3)
					best = 5;
				else if (p1 == 2 && p2 == 0)
					best = 3;
				else if (p1 == 0 && p2 == -2)
					best = 3;

				if (best > (*parent) -> value -> best_score) {
					(*parent) -> value -> best_score = best;
					//printf("BEST SCORE MAX SCORE %d MOVE %d\n", best, move);
				}
			}
		}
	}
	//}
}
