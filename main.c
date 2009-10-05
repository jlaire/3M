#include <stdlib.h>
#include <stdio.h>

#include "const.h"
#include "ruleset.h"
#include "move.h"
#include "position.h"
#include "indexing.h"
#include "database.h"

#define BUF_SIZE 1024

int main(void) {
	init_adjacent();
	init_neighbours();
	init_positions();
	init_dead_pattern_table();
	init_indexing_tables();

/*	generate_database(STANDARD);
	generate_database(BORDER);
	generate_database(HORIZONTAL);*/

	char buf[BUF_SIZE];
	position_t position = start_position;

	for (;;) {
		show_position(position, buf, BUF_SIZE);
		fprintf(stderr, "%s", buf);

		move_t move_list[MAX_BRANCHING];
		int n = list_legal_moves(position, move_list);
		fprintf(stderr, "[");
		for (int i = 0; i < n; ++i) {
			int temp = show_move(move_list[i], buf, BUF_SIZE);
			if (temp < 0) {
				fprintf(stderr, "ERROR: %d\n", temp);
				return EXIT_FAILURE;
			}
			fprintf(stderr, (i == n - 1 ? "%s" : "%s, "), buf);
		}
		fprintf(stderr, "]\n");

		move_t undo_list[MAX_BRANCHING];
		n = list_legal_undos(position, undo_list);
		fprintf(stderr, "[");
		for (int i = 0; i < n; ++i) {
			int temp = show_move(undo_list[i], buf, BUF_SIZE);
			if (temp < 0) {
				fprintf(stderr, "ERROR: %d\n", temp);
				return EXIT_FAILURE;
			}
			fprintf(stderr, (i == n - 1 ? "%s" : "%s, "), buf);
		}
		fprintf(stderr, "]\n");

		READ_INPUT:
		printf("> ");
		if (fgets(buf, BUF_SIZE, stdin) == NULL)
			break;

		move_t move;

		if (buf[0] == '?') {
			solve(position);
			result_t result = lookup(position);
			if (result_ok(result)) {
				show_result(result, buf, BUF_SIZE);
				fprintf(stderr, "%s\n", buf);
			}
			goto READ_INPUT;
		}

		if (buf[0] == '<') {
			int temp = read_move(&move, buf + 1);
			if (temp < 0 || !undo_valid_and_legal(position, move)) {
				fprintf(stderr, "Illegal undo\n");
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

		int temp = read_move(&move, buf);
		if (temp < 0 || !move_valid_and_legal(position, move)) {
			fprintf(stderr, "Illegal move\n");
			continue;
		}
		if (show_move(move, buf, BUF_SIZE) >= 0)
			fprintf(stderr, "(%s)\n", buf);
		position = apply_move(position, move);
	}

	return EXIT_SUCCESS;
}
