#include <stdio.h>
#include <stdlib.h>
#include <assert.h>

#include "const.h"
#include "ruleset.h"
#include "position.h"
#include "indexing.h"
#include "database.h"

const result_t error_result = RESULT_ERROR_BIT;
const result_t ok_result = 0;

int result_ok(result_t result) {
	return (result & RESULT_ERROR_BIT) == 0;
}

enum player result_get_winner(result_t result) {
	return result & RESULT_WINNER_BIT ? ENEMIES : MUSKETEERS;
}

result_t result_set_winner(result_t result, enum player player) {
	if (player == MUSKETEERS)
		return result & ~RESULT_WINNER_BIT;
	else if (player == ENEMIES)
		return result | RESULT_WINNER_BIT;
	else
		return error_result;
}

int result_get_depth(result_t result) {
	return result & RESULT_DEPTH_MASK;
}

result_t result_set_depth(result_t result, int depth) {
	return result & ~RESULT_DEPTH_MASK | depth & RESULT_DEPTH_MASK;
}

/* Musketeers can force a win in N moves */
int show_result(result_t result, char *string, int N) {
	if (!result_ok(result))
		return -1;

	if (N < 39)
		return -2;

	return sprintf(string, "%s can force a win in %d moves",
	               result_get_winner(result) == MUSKETEERS
	                   ? "Musketeers" : "Enemies",
	               result_get_depth(result));
}

result_t max_result(enum player player, result_t r0, result_t r1) {
	enum player w0 = result_get_winner(r0);
	enum player w1 = result_get_winner(r1);
	int d0 = result_get_depth(r0);
	int d1 = result_get_depth(r1);

	if (!result_ok(r0))
		return r1;
	if (!result_ok(r1))
		return r0;

	if (w0 == player)
		if (w1 == player)
			return d0 < d1 ? r0 : r1;
		else
			return r0;
	else
		if (w1 == player)
			return r1;
		else
			return d0 > d1 ? d0 : d1;
}

#define MAX_FILENAME 18
const char *ruleset_strs[] = {"STANDARD", "BORDER", "HORIZONTAL"};
const char *player_strs[] = {"", "M", "E"};

void make_filename(char *string, enum ruleset ruleset, int enemies, enum player player) {
	sprintf(string,
	        "DB/%s/%02d%s",
	        ruleset_strs[ruleset],
	        enemies,
	        player_strs[player]);
}

void make_next_filename(char *string, enum ruleset ruleset,
                        int enemies, enum player player)
{
	assert(player == MUSKETEERS || player == ENEMIES);
	if (player == MUSKETEERS)
		make_filename(string, ruleset, enemies - 1, ENEMIES);
	else if (player == ENEMIES)
		make_filename(string, ruleset, enemies, MUSKETEERS);
}		

/* This is the base case. There can't be any legal moves in any
 * positions with 0 enemies, so we don't depend on earlier depths.
 */
void solve_zero_enemies(enum ruleset ruleset, enum player player) {
	char filename[MAX_FILENAME];
	uint64_t index;
	struct position position;
	FILE *out;
	enum player temp;
	result_t result;

	position.ruleset = ruleset;
	position.player_to_move = player;

	make_filename(filename, ruleset, 0, player);
	if ((out = fopen(filename, "wb")) == NULL) {
		fprintf(stderr, "solve_zero_enemies: Couldn't open file %s\n", filename);
		exit(EXIT_FAILURE);
		return;
	}

	result = result_set_depth(ok_result, 0);

	for (index = 0; index < indices[0]; ++index) {
		index_to_position(&position, 0, index);
		temp = winner(&position);
		assert(temp == MUSKETEERS || temp == ENEMIES);
		result = result_set_winner(result, temp);
		fprintf(out, "%c", (unsigned char)result);
	}

	fclose(out);
}

void solve(enum ruleset ruleset, int enemies, enum player player) {
	char filename[MAX_FILENAME];
	uint64_t index;
	struct position position;
	FILE *next;
	FILE *out;
	enum player temp_player;
	result_t result;
	result_t temp_result;
	result_t *next_results;
	size_t size;
	struct move move_list[MAX_BRANCHING];
	int i;
	int n;

	position.ruleset = ruleset;
	position.player_to_move = player;

	make_next_filename(filename, ruleset, enemies, player);
	if ((next = fopen(filename, "rb")) == NULL) {
		fprintf(stderr, "solve: Couldn't open file %s\n", filename);
		exit(EXIT_FAILURE);
		return;
	}

	size = indices[player == MUSKETEERS ? enemies - 1 : enemies];
	assert((next_results = malloc(size * sizeof(result_t))) != NULL);
	if (fread(next_results, sizeof(result_t), size, next) != size) {
		fprintf(stderr, "solve: Couldn't read file %s\n", filename);
		exit(EXIT_FAILURE);
		return;
	}

	fclose(next);

	make_filename(filename, ruleset, enemies, player);
	if ((out = fopen(filename, "wb")) == NULL) {
		fprintf(stderr, "solve: Couldn't open file %s\n", filename);
		exit(EXIT_FAILURE);
		return;
	}

	for (index = 0; index < indices[enemies]; ++index) {
		index_to_position(&position, enemies, index);
		result = error_result;
		if ((temp_player = winner(&position)) != NOBODY) {
			result = result_set_winner(result, temp_player);
			result = result_set_depth(result, 0);
		}
		else {
			n = list_legal_moves(&position, move_list);
			for (i = 0; i < n; ++i) {
/*				char buf[128];
				struct position temp_pos;
				temp_pos.player_to_move = player;
				index_to_position(&temp_pos, enemies, index);
				show_position(&temp_pos, buf, 128);
				fprintf(stderr, "%s", buf);
				show_position(&position, buf, 128);
				fprintf(stderr, "%s", buf);*/
				apply_move(&position, &move_list[i]);
/*				show_position(&position, buf, 128);
				fprintf(stderr, "%s\n", buf);
				getchar();*/
				temp_result = next_results[position_to_index(&position)];
				result = max_result(player, result, temp_result);
				apply_undo(&position, &move_list[i]);
			}
		}

		fputc((int)result, out);
	}

	fclose(out);
}

void generate_database(enum ruleset ruleset) {
	int enemies;

	fprintf(stderr, "\tenemies = 0\n");
	solve_zero_enemies(ruleset, MUSKETEERS);
	solve_zero_enemies(ruleset, ENEMIES);

	for (enemies = 1; enemies <= MAX_ENEMIES; ++enemies) {
		fprintf(stderr, "\tenemies = %d\n", enemies);
		solve(ruleset, enemies, MUSKETEERS);
		solve(ruleset, enemies, ENEMIES);
	}
}

result_t lookup(struct position *position) {
	FILE *file;
	char filename[MAX_FILENAME];
	uint64_t index;
	int temp;

	make_filename(filename, position->ruleset,
	              count_enemies(position), position->player_to_move);
	if ((file = fopen(filename, "rb")) == NULL) {
		fprintf(stderr, "lookup: Couldn't open file %s\n", filename);
		exit(EXIT_FAILURE);
		return error_result;
	}

	index = position_to_index(position);
	if (fseek(file, (long)index, SEEK_SET)) {
		fprintf(stderr, "lookup: Seek failed\n");
		exit(EXIT_FAILURE);
		return error_result;
	}

	if ((temp = fgetc(file)) == EOF) {
		fprintf(stderr, "lookup: fgetc failed\n");
		exit(EXIT_FAILURE);
		return error_result;
	}

	return (result_t)temp;
}
