#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <assert.h>
#include <sys/types.h>
#include <sys/stat.h>

#include "const.h"
#include "position.h"
#include "indexing.h"
#include "database.h"

const result_t error_result = RESULT_ERROR_BIT;
const result_t ok_result = 0;

int show_result(result_t result, char *string, int N) {
	if (!result_ok(result))
		return -1;

	if (N < 39)
		return -2;

	int depth = result_get_depth(result);
	return sprintf(string, "%s can force a win in %d move%s",
	               result_get_winner(result) == MUSKETEERS ?
	                   "Musketeers" : "Enemies",
	               depth,
	               depth == 1 ? "" : "s");
}

int show_result_short(result_t result, char *string, int N) {
	if (N < 4)
		return -2;

	if (!result_ok(result))
		return sprintf(string, "?");

	enum player winner = result_get_winner(result);
	return sprintf(string, "%c%d",
	               winner == MUSKETEERS ? 'M' : 'E',
	               result_get_depth(result));
}

result_t max_result(enum player player, result_t r0, result_t r1) {
	if (!result_ok(r0))
		return r1;
	if (!result_ok(r1))
		return r0;

	enum player w0 = result_get_winner(r0);
	enum player w1 = result_get_winner(r1);
	int d0 = result_get_depth(r0);
	int d1 = result_get_depth(r1);

	if (w0 == player) {
		if (w1 == player)
			return d0 < d1 ? r0 : r1;
		else
			return r0;
	}
	else {
		if (w1 == player)
			return r1;
		else
			return d0 > d1 ? r0 : r1;
	}
}

#define MAX_FILENAME 8
static void make_filename(char *string, int enemies, enum player player) {
	static const char database_dir[] = "data";
	static const char *player_strs[] = {"M", "E", ""};
	sprintf(string,
	        "%s/%02d%s",
		database_dir,
	        enemies,
	        player_strs[player]);
}

static inline int get_next_enemies(int enemies, enum player player) {
	return player == MUSKETEERS ? enemies - 1 : enemies;
}

static inline int get_next_turn(enum player player) {
	return player == MUSKETEERS ? ENEMIES : MUSKETEERS;
}

/* Returns the size in bytes on success, 0 on failure */
static uint64_t file_size(const char *filename) {
	struct stat buf;

	if (stat(filename, &buf))
		return 0;

	return buf.st_size;
}

/* This is the base case. There can't be any legal moves without enemies */
static int generate_file_zero_enemies(enum player turn) {
	position_t position = start_position;
	position = set_turn(position, turn);

	char filename[MAX_FILENAME];
	make_filename(filename, 0, turn);

	if (file_size(filename) == indices[0])
		return 0;

	fprintf(stderr, "Generating file %s...", filename);
	fflush(stderr);
	FILE *out;
	if ((out = fopen(filename, "ab")) == NULL) {
		fprintf(stderr, "generate_file_zero_enemies: Couldn't open file %s for writing\n", filename);
		return -1;
	}

	result_t result = result_set_depth(ok_result, 0);

	for (uint64_t index = file_size(filename); index < indices[0]; ++index) {
		position = index_to_position(position, 0, index);
		enum player temp = winner(position);
		assert(temp == MUSKETEERS || temp == ENEMIES);
		result = result_set_winner(result, temp);
		fputc((int)result, out);
	}

	fclose(out);

	fprintf(stderr, " done\n");

	return 0;
}

static int generate_file(int enemies, enum player turn) {
	if (enemies == 0)
		return generate_file_zero_enemies(turn);

	char filename[MAX_FILENAME];
	make_filename(filename, enemies, turn);
	if (file_size(filename) == indices[enemies])
		return 0;

	int next_enemies = get_next_enemies(enemies, turn);
	enum player next_turn = get_next_turn(turn);
	char next_filename[MAX_FILENAME];
	make_filename(next_filename, next_enemies, next_turn);

	// Generate necessary files recursively
	if (file_size(next_filename) != indices[next_enemies])
		if (generate_file(next_enemies, next_turn) == -1)
			return -1;

	FILE *next;
	if ((next = fopen(next_filename, "rb")) == NULL) {
		fprintf(stderr, "generate_file: Couldn't open file %s for reading\n", next_filename);
		return -1;
	}

	size_t size = indices[turn == MUSKETEERS ? enemies - 1 : enemies];
	result_t *next_results = malloc(size * sizeof *next_results);
	assert(next_results != NULL);
	if (fread(next_results, sizeof(result_t), size, next) != size) {
		fprintf(stderr, "generate_file: Couldn't read file %s\n", next_filename);
		free(next_results);
		return -1;

	}

	fclose(next);

	make_filename(filename, enemies, turn);

	fprintf(stderr, "Generating file %s...", filename);
	fflush(stderr);
	FILE *out;
	if ((out = fopen(filename, "ab")) == NULL) {
		fprintf(stderr, "generate_file: Couldn't open file %s for writing\n", filename);
		return -1;
	}

	position_t position = start_position;
	position = set_turn(position, turn);

	for (uint64_t index = file_size(filename); index < indices[enemies]; ++index) {
		position = index_to_position(position, enemies, index);
		result_t result = error_result;
		enum player temp;
		if ((temp = winner(position)) != NOBODY) {
			result = result_set_winner(ok_result, temp);
			result = result_set_depth(result, 0);
		}
		else {
			move_t move_list[MAX_BRANCHING];
			int n = list_legal_moves(position, move_list);
			for (int i = 0; i < n; ++i) {
				position_t temp_pos = apply_move(position, move_list[i]);
				result_t temp_result = next_results[position_to_index(temp_pos)];
				result = max_result(turn, result, result_inc_depth(temp_result));
			}
		}

		fputc((int)result, out);
	}

	free(next_results);
	fclose(out);

	fprintf(stderr, " done\n");

	return 0;
}

int generate_database(void) {
	for (int enemies = 0; enemies <= MAX_ENEMIES; ++enemies) {
		int temp;
		if ((temp = generate_file(enemies, MUSKETEERS)) == -1)
			return temp;
		if ((temp = generate_file(enemies, ENEMIES)) == -1)
			return temp;
	}

	return 0;
}

int solve(position_t position) {
	position = normalize_position(position);
	int enemies = count_enemies(position);
	enum player turn = get_turn(position);

	return generate_file(enemies, turn);
}

result_t lookup(position_t position) {
	if (count_musketeers(position) != 3)
		return error_result;

	position = normalize_position(position);

	int enemies = count_enemies(position);
	enum player turn = get_turn(position);

	char filename[MAX_FILENAME];
	make_filename(filename, enemies, turn);

	FILE *file = fopen(filename, "rb");
	if (file == NULL) {
		fprintf(stderr, "lookup: Couldn't open file %s for reading\n", filename);
		return error_result;
	}

	uint64_t index = position_to_index(position);
	if (fseek(file, (long)index, SEEK_SET)) {
		fprintf(stderr, "lookup: fseek failed\n");
		return error_result;
	}

	int c = fgetc(file);
	if (c == EOF) {
		fprintf(stderr, "lookup: fgetc failed\n");
		return error_result;
	}

	return (result_t)c;
}
