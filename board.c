#include "board.h"
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>

#define KRED  "\x1B[31m" // Player 1
#define KBLU  "\x1B[34m" // Player 2
#define UNDERLINE  "\033[4m"
#define RESET "\033[0m"
//https://stackoverflow.com/questions/3585846/color-text-in-terminal-aplications-in-unix

board* init_board(int num_rows, int num_cols, int r)
{
	board* b = malloc(sizeof(board));
	b -> row_len = num_rows;
	b -> column_len = num_cols;
	b -> r = r;
	b -> size = num_rows * num_cols;
	b -> best_score = 0;
	b -> move = -1;

	int size = b->size;
	b->array = malloc(sizeof(b->array) * size);
	memset(b->array, 0, sizeof(b->array) * size);
	return b;
}

board* copy_board(board* original)
{
	board* new_board = malloc(sizeof(board));
	new_board -> row_len = original -> row_len;
	new_board -> column_len = original -> column_len;
	int size = original -> size;
	new_board -> size = size;
	new_board -> best_score = 0;
	new_board -> move = -1;

	new_board -> r = original -> r;
	new_board -> array = malloc(sizeof(original->array) * size);
	memset(new_board->array, 0, sizeof(new_board->array) * size);

	int i;
	for (i = 0; i <= size; i++) {
		new_board->array[i] = original->array[i];
	}
	return new_board;
}

void delete_board(board* b)
{
	free(b->array);
	free(b);
}

int add_checker(board* b, int column, int player)
{
	int start = column;
	int end = start + (b->row_len - 1) * b->column_len;
	int i;
	// Start at end of array
	// If column is empty, add checker
	for (i = end; i >= start; i -= b->column_len) {
		if (b->array[i] == 0) {
			b->array[i] = player;
			b->move = column;
			return 0;
		}
	}
	return 1;
}

int check_horizontal(board* b)
{
	int row_size = b->column_len;
	int col_size = b->row_len;
	int i;

	for (i = 0; i < col_size; i++) {
		int j;
		int p1 = 0;
		int p2 = 0;
		for (j = 0; j < row_size; j++) {
			int current = b->array[(i*row_size) + j];
			int result = check_owner(current, &p1, &p2, b->r);
			if (result > 0)
				return result;
		}
	}
	return 0;
}

int check_vertical(board* b)
{
	int row_size = b->column_len;
	int col_size = b->row_len;
	int i;

	for (i = 0; i < row_size; i++) {
		int j;
		int p1 = 0;
		int p2 = 0;
		for (j = 0; j < col_size; j++) {
			int current = b->array[i + (j * row_size)];
			int result = check_owner(current, &p1, &p2, b->r);
			if (result > 0)
				return result;
		}
	}
	return 0;
}

int check_forward_diag(board* b)
{
	int i, j;
	int delta = b->column_len + 1;

	/* First half, rows first */
	int first_row = b->column_len - b->r - 1;
	int last_row = 0;

	for (i = first_row; i >= last_row; i--) {
		j = get_index(b->column_len, i, 0);
		int p1 = 0;
		int p2 = 0;
		while (j < b->size) {
			int current = b->array[j];
			int result = check_owner(current, &p1, &p2, b->r);
			if (result > 0)
				return result;
			j += delta;
		}
	}
	/* Second half */
	first_row = 0;
	last_row = b->r - 1;
	int first_column = 1;
	int last_column = b->row_len;

	for (i = first_row; i < last_row; i++) {
		int temp = 0;
		int index = 0;
		int p1 = 0;
		int p2 = 0;
		for (j = first_column; j <= last_column; j++) {
			index = get_index(b->column_len, temp, j);
			int current = b->array[index];
			int result = check_owner(current, &p1, &p2, b->r);
			if (result > 0)
				return result;
			temp++;
		}
		first_column += 1;
	}

	return 0;
}

int check_backwards_diag(board* b)
{
	int i, j;
	int delta = b->row_len;

	/* First half, rows first */
	int first_row = b->r - 1;
	int last_row = b->row_len - 1;
	for (i = first_row; i <= last_row; i++) {
		j = get_index(b->column_len, i, 0);
		int p1 = 0;
		int p2 = 0;
		while (j > 0) {
			int current = b->array[j];
			int result = check_owner(current, &p1, &p2, b->r);
			if (result > 0)
				return result;
			j -= delta;
		}
	}

	/* Second half */
	first_row = b->row_len -1;
	last_row = 0;
	int first_column = 1;
	int last_column = b->column_len-1;

	for (i = first_row; i >= last_row; i--) {
		int temp = first_row;
		int index = 0;
		int p1 = 0;
		int p2 = 0;
		for (j = first_column; j <= last_column; j++) {
			index = get_index(b->column_len, temp, j);
			int current = b->array[index];
			int result = check_owner(current, &p1, &p2, b->r);
			if (result > 0)
				return result;
			temp--;
		}
		first_column += 1;
		last_row += 1;
	}

	return 0;
}

int terminal_test(board *b)
{
	int win = 0;
	if((win = check_horizontal(b)) > 0) {
		return win;
	} else if ((win = check_vertical(b)) > 0) {
		return win;
	} else if ((win = check_forward_diag(b)) > 0) {
		return win;
	} else if ((win = check_backwards_diag(b)) > 0) {
		return win;
	}
	return win;
}

int get_index(int col, int i, int j)
{
	return i * col + j;
}

int check_owner(int current, int* p1, int* p2, int r)
{

	if (current == 1) {
		*p1+=1;
		*p2=0;
	} else if (current == 2) {
		*p1=0;
		*p2+=1;
	} else if (current == 0) {
		*p1=0;
		*p2=0;
		return 0;
	}

	if (*p1 == r) {
		return 1;
	} else if (*p2 == r) {
		return 2;
	} else {
		return 0;
	}
}

void swap(int* player)
{
	if (*player == 1)
		*player = 2;
	else
		*player = 1;
}

void print_board(board* b)
{
	int size = b -> size;
	int mod = b -> column_len;
	int i, j;

	// Print columns
	printf(UNDERLINE " |" RESET);
	for (i = 0; i < mod; i++) {
		printf(UNDERLINE " %d" RESET, i);
	}
	j = 0;
	printf("\n");

	// Print checkers
	for (i = 0; i < size; i++)
	{
		if (i % mod == 0) {
			printf("%d| ", j++);
		}
		if (b->array[i] == 1) {
			printf(KRED "%d " RESET, b->array[i]);
		} else if (b->array[i] == 2) {
			printf(KBLU "%d " RESET, b->array[i]);
		} else {
			printf("%d ", b->array[i]);
		}

		if ((i+1) % mod == 0)
		{
			printf("\n");
		}
	}

	for (j = 0; j < 16; j++)
		printf(UNDERLINE " " RESET);
	printf("\n");
}

int compare_board(board* one, board* two)
{
	int* arr_one = one -> array;
	int* arr_two = two -> array;

	if (memcmp(arr_one, arr_two, sizeof(arr_one) * one->size) == 0) {
		return 0;
	} else {
		return 1;
	}
}

