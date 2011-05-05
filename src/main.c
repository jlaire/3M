#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>

#include "const.h"
#include "move.h"
#include "position.h"
#include "indexing.h"
#include "database.h"

#define BUF_SIZE 1024

/* read-eval-print loop */
static void repl(void) {
	char buf[BUF_SIZE];
	position_t position = start_position;

	for (;;) {
		show_position(position, buf, BUF_SIZE);
		printf("%s", buf);

		move_t move_list[MAX_BRANCHING];
		int n = list_legal_moves(position, move_list);
		printf("[");
		for (int i = 0; i < n; ++i) {
			int temp = show_move(move_list[i], buf, BUF_SIZE);
			if (temp < 0) {
				fprintf(stderr, "ERROR: %d\n", temp);
				exit(EXIT_FAILURE);
			}
			if (i == n - 1)
				printf("%s", buf);
			else
				printf("%s, ", buf);
		}
		printf("]\n");

		move_t undo_list[MAX_BRANCHING];
		n = list_legal_undos(position, undo_list);
		printf("[");
		for (int i = 0; i < n; ++i) {
			int temp = show_move(undo_list[i], buf, BUF_SIZE);
			if (temp < 0) {
				fprintf(stderr, "ERROR: %d\n", temp);
				exit(EXIT_FAILURE);
			}
			if (i == n - 1)
				printf("%s", buf);
			else
				printf("%s, ", buf);
		}
		printf("]\n");

		READ_INPUT:
		printf("> ");
		if (fgets(buf, BUF_SIZE, stdin) == NULL)
			break;

		if (buf[0] == '?') {
			solve(position);
			result_t result = lookup(position);
			if (result_ok(result)) {
				show_result(result, buf, BUF_SIZE);
				printf("%s\n", buf);
			}
			goto READ_INPUT;
		}

		if (buf[0] == '<') {
			move_t move;
			int temp = read_move(&move, buf + 1);
			if (temp < 0 || !undo_valid_and_legal(position, move)) {
				fprintf(stderr, "Illegal undo: `%s'\n", buf + 1);
				continue;
			}
			if (show_move(move, buf, BUF_SIZE) >= 0)
				fprintf(stderr, "(%s)\n", buf);
			position = apply_undo(position, move);
			continue;
		}

		if (buf[0] == '.') {
			read_position(&position, buf + 1);
			continue;
		}

		move_t move;
		int temp = read_move(&move, buf);
		if (temp < 0 || !move_valid_and_legal(position, move)) {
			fprintf(stderr, "Illegal move: `%s'\n", buf);
			continue;
		}
		if (show_move(move, buf, BUF_SIZE) >= 0)
			printf("(%s)\n", buf);
		position = apply_move(position, move);
	}
}

static void batch_mode(void) {
	char line[BUF_SIZE];
	while (fgets(line, BUF_SIZE, stdin) != NULL) {
		position_t position;
		if (read_position(&position, line) == -1) {
			printf("ERROR\n");
			continue;
		}
		result_t result = lookup(position);
		char str[BUF_SIZE] = {0};
		show_result_short(result, str, BUF_SIZE);
		printf("%s -> ", str);
		move_t moves[MAX_BRANCHING];
		int move_count = list_legal_moves(position, moves);
		int strlen = 0;
		for (int i = 0; i < move_count; ++i) {
			result_t result = lookup(apply_move(position, moves[i]));
			if (i > 0) {
				str[strlen++] = ',';
				str[strlen++] = ' ';
			}
			strlen += show_move(moves[i], str + strlen, BUF_SIZE - strlen);
			str[strlen++] = ' ';
			strlen += show_result_short(result, str + strlen, BUF_SIZE - strlen);
		}
		str[strlen] = '\0';
		printf("[%s]\n", str);
	}
}

static void usage(void) {
	printf("Usage: ./3M [-g] [-i|-s]\n"
	       "\t-g\tGenerate databases for optimal AI\n"
	       "\t-h\tShow usage\n"
	       "\t-i\tInteractive mode\n"
	       "\t-s\tRead positions from stdin and write optimal moves to stdout\n");
}

int main(int argc, char *argv[]) {
	/* Initialize lookup tables. This is fairly cheap */
	init_adjacent();
	init_neighbours();
	init_positions();
	init_indexing_tables();

	int option_g = 0;
	int option_h = 0;
	int option_i = 0;
	int option_s = 0;

	int option;
	while ((option = getopt(argc, argv, "ghis")) != -1) {
		switch (option) {
		case 'i':
			option_i = 1;
			break;
		case 'g':
			option_g = 1;
			break;
		case 's':
			option_s = 1;
			break;
		case 'h':
		default:
			option_h = 1;
			break;
		}
	}

	if (!option_i && !option_g && !option_s)
		option_h = 1;

	// Both -i and -s use stdin
	if (option_i && option_s)
		option_h = 1;

	if (option_h) {
		usage();
		exit(EXIT_FAILURE);
	}

	if (option_g)
		generate_database();
	if (option_i)
		repl();
	if (option_s)
		batch_mode();

	return EXIT_SUCCESS;
}
