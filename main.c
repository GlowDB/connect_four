#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <time.h>
#include "board.h"
#include "linked_list.h"
#include "tree.h"

int play(board* starting_board, int r, tree* game_tree);
void error(char* msg);

int main(int argc, char* argv[])
{
	/* Error handling for command line arguments */
	if (argc != 4) { error("usage -- ./main n m r"); }

	/* Initialize board */
	int num_rows = strtol(argv[1], NULL, 10);
	int num_cols = strtol(argv[2], NULL, 10);
	int r = strtol(argv[3], NULL, 10);
	board* b = init_board(num_rows, num_cols, r);

	/* Initialize game tree */
	tree* game_tree = create_tree();
	set_root(game_tree, b);

	/* Start game */
	int win = play(b, r, game_tree);

	/* End game */
	if (system("clear") > 0){}
	if (win > 0)
		printf("Winner: %d\n", win);
	else
		printf("Cat's game\n");
	print_board(game_tree->root->value);

	/* Cleanup */
	delete_tree(game_tree);
	return 0;
}

int play(board* b, int r, tree* game_tree)
{
	// Sentinel variable
	int win = 0;
	// Current player
	int player = 1;

	// Randomize first and second moves
	srand(time(NULL));
	add_checker(game_tree->root->value, (rand() % 7), 1);
	add_checker(game_tree->root->value, (rand() % 7), 2);

	// Loop until win condition is met
	while (win == 0) {
		// Print current game board
		print_board(b);

		// Generate game tree
		struct list_node* root = game_tree->root;

		// Store player input, best-scoring move, and best column
		int input, best, best_column;

		// Player 1 is AI
		if (player == 1) {
			/*printf("Input move: ");
			if (scanf("%d", &input)){}
			best_column = input;*/

			generate_permutations(&game_tree->root, game_tree->root->value, 0, 0);
			root -> value -> best_score = -999;
			max_decision(&root);
			best = root -> value -> best_score;
			best_column = root -> value -> move;
			printf("Best move for player %d: Score %d Column %d\n", player, best, best_column);

		} else {
			/*printf("Input move: ");
			if (scanf("%d", &input)){}
			best_column = input;
			*/
			generate_permutations(&game_tree->root, game_tree->root->value, 0, 1);
			root -> value -> best_score = 999;
			min_decision(&root);
			best = root -> value -> best_score;
			best_column = root -> value -> move;
			printf("Best move for player %d: Score %d Column %d\n", player, best, best_column);

		}

		// Verify that the move was valid and that the column could be added to
		if (add_checker(root->value, best_column, player) == 1) {
			if (terminal_test(b) != -1) {
				printf("Invalid move for player %d. Retry last turn\n", player);
				continue;
			} else {
				win = terminal_test(b);
			}
		} else {
			swap(&player);
		}

		// Delete the game board
		delete_permutations(&game_tree, &b);
		//if (system("clear")){}

		// Check win condition
		win = terminal_test(b);
	}
	return win;
}

void error(char* msg)
{
	printf("%s\n", msg);
	exit(1);
}
