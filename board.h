/*
 * board.h
 *
 *  Created on: Sep 27, 2015
 *      Author: rob
 */
#ifndef BOARD_H_
#define BOARD_H_

typedef struct board {
	int *array;
	int row_len;
	int column_len;
	int r;
	int size;
	int best_score;
	int move;
} board;

/*
 * Initialization functions
 */
board* init_board();
board* copy_board(board* b);
void delete_board(board* b);

/*
 * Solver functions
 */
int check_horizontal(board* b);
int check_vertical(board* b);
int check_forward_diag(board* b);
int check_backwards_diag(board* b);
int terminal_test(board* b);

/*
 * Utility functions
 */
/* Get index for i, j */
int get_index(int col, int i, int j);
/* Checker owner */
int check_owner(int current, int* p1, int* p2, int r);
/* Switch players */
void swap(int* player);
/* Print board contents */
void print_board(board* b);
/* Add checker to specified column */
int add_checker(board* b, int column, int player);
/* Compare two board arrays */
int compare_board(board* one, board* two);

#endif /* BOARD_H_ */
