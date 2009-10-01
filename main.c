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
	struct position position;
	struct move move_list[MAX_BRANCHING];
	struct move undo_list[MAX_BRANCHING];
	struct move move;
	char buf[BUF_SIZE];
	int i;
	int n;
	int temp;

	int enemies;
	uint64_t index;
	uint64_t index2;
	uint64_t print_at;
	enum ruleset ruleset;
	enum player player;
	result_t result;

	init_adjacent();
	init_adjacents();
	init_dead_pattern_table();
	init_indexing_tables();

	generate_database(STANDARD);
	generate_database(BORDER);
	generate_database(HORIZONTAL);

	for (ruleset = 0; ruleset < RULESETS; ++ruleset) {
		position.ruleset = ruleset;
		for (player = MUSKETEERS; player <= ENEMIES; ++player) {
			position.player_to_move = player;
			for (index = 0; index < indices[0]; ++index) {
				index_to_position(&position, 0, index);
				show_position(&position, buf, BUF_SIZE);
				fprintf(stderr, "%s", buf);
				result = lookup(&position);
				show_result(result, buf, BUF_SIZE);
				fprintf(stderr, "%s\n", buf);
				index2 = position_to_index(&position);
				if (index2 != index) {
					fprintf(stderr, "ERROR: %ld != %ld\n",
					        (long)index, (long)index2);
					return EXIT_FAILURE;
				}
				getchar();
			}
		}
	}
	return EXIT_SUCCESS;

	position.ruleset = STANDARD;
	position.player_to_move = MUSKETEERS;
	for (enemies = 0; enemies <= 0/*MAX_ENEMIES*/; ++enemies) {
		fprintf(stderr, "  %d (%ld)\n", enemies, (long)indices[enemies]);
		print_at = 1;
		for (index = 0; index < indices[enemies]; ++index) {
			index_to_position(&position, enemies, index);
/*			if (show_position(&position, buf, BUF_SIZE) < 0) {
				fprintf(stderr, "show_position failed\n");
				return EXIT_FAILURE;
			}
			fprintf(stderr, "%s\n", buf);*/
			index2 = position_to_index(&position);
			if (index2 != index) {
				fprintf(stderr, "ERROR: %ld != %ld\n", (long)index, (long)index2);
				return EXIT_FAILURE;
			}
			if (index >= print_at) {
				fprintf(stderr, "    %ld\n", (long)print_at);
				print_at *= 2;
			}
		}
	}

	position.ruleset = STANDARD;
	set_to_start_position(&position);

	for (;;) {
		show_position(&position, buf, BUF_SIZE);
		fprintf(stderr, "%s", buf);
/*		fprintf(stderr, "%ld\n", position_to_index(&position));*/
		n = list_legal_moves(&position, move_list);
		fprintf(stderr, "[");
		for (i = 0; i < n; ++i) {
			if ((temp = show_move(&move_list[i], buf, BUF_SIZE)) < 0) {
				fprintf(stderr, "ERROR: %d\n", temp);
				return EXIT_FAILURE;
			}
			fprintf(stderr, (i == n - 1 ? "%s" : "%s, "), buf);
		}
		fprintf(stderr, "]\n");

		n = list_legal_undos(&position, undo_list);
		fprintf(stderr, "[");
		for (i = 0; i < n; ++i) {
			if ((temp = show_move(&undo_list[i], buf, BUF_SIZE)) < 0) {
				fprintf(stderr, "ERROR: %d\n", temp);
				return EXIT_FAILURE;
			}
			fprintf(stderr, (i == n - 1 ? "%s" : "%s, "), buf);
		}
		fprintf(stderr, "]\n");

		printf("> ");
		if (fgets(buf, BUF_SIZE, stdin) == NULL)
			break;
		if ((temp = read_move(&move, buf)) < 0 || !move_legal(&position, &move)) {
			fprintf(stderr, "Illegal move\n");
			continue;
		}
		if (show_move(&move, buf, BUF_SIZE) >= 0)
			fprintf(stderr, "(%s)\n", buf);
		apply_move(&position, &move);
	}

	return EXIT_SUCCESS;
}
