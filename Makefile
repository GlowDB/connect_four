main: main.c tree.c linked_list.c board.c
	gcc -g -Wall -o main main.c linked_list.c tree.c board.c -O3
