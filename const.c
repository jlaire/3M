#include "const.h"
#include "ruleset.h"
#include <math.h>

int adjacent[SQUARES][SQUARES] = {{0}};

void init_adjacent(void) {
	int i, i_x, i_y;
	int j, j_x, j_y;

	for (i = 0; i < SQUARES; ++i) {
		i_x = i % 5;
		i_y = i / 5;
		for (j = 0; j < SQUARES; ++j) {
			j_x = j % 5;
			j_y = j / 5;
			if (i_x == j_x && fabs(i_y - j_y) == 1 ||
			    i_y == j_y && fabs(i_x - j_x) == 1)
			{
				adjacent[i][j] = 1;
				adjacent[j][i] = 1;
			}
		}
	}
}

int adjacents[SQUARES][MAX_ADJACENTS];
void init_adjacents(void) {
	int i;
	int j;
	int k;

	for (i = 0; i < SQUARES; ++i) {
		k = 0;
		for (j = 0; j < SQUARES; ++j)
			if (adjacent[i][j])
				adjacents[i][k++] = j;
		for (; k < MAX_ADJACENTS; ++k)
			adjacents[i][k] = -1;
	}
}

int dead_pattern_table[RULESETS][SQUARES][SQUARES][SQUARES] = {{{{0}}}};

void init_dead_pattern_table(void) {
	int i, i_x, i_y;
	int j, j_x, j_y;
	int k, k_x, k_y;

	/* STANDARD */
	for (i = 0; i < SQUARES; ++i) {
		i_x = i % 5;
		i_y = i / 5;
		for (j = i + 1; j < SQUARES; ++j) {
			j_x = j % 5;
			j_y = j / 5;
			for (k = j + 1; k < SQUARES; ++k) {
				k_x = k % 5;
				k_y = k / 5;
				if (i_x == j_x && j_x == k_x ||
				    i_y == j_y && j_y == k_y)
				{
					dead_pattern_table[STANDARD][i][j][k] = 1;
					dead_pattern_table[STANDARD][i][k][j] = 1;
					dead_pattern_table[STANDARD][j][i][k] = 1;
					dead_pattern_table[STANDARD][j][k][i] = 1;
					dead_pattern_table[STANDARD][k][i][j] = 1;
					dead_pattern_table[STANDARD][k][j][i] = 1;
				}
			}
		}
	}

	/* BORDER */
	for (i = 0; i < SQUARES; ++i) {
		i_x = i % 5;
		i_y = i / 5;
		for (j = i + 1; j < SQUARES; ++j) {
			j_x = j % 5;
			j_y = j / 5;
			for (k = j + 1; k < SQUARES; ++k) {
				k_x = k % 5;
				k_y = k / 5;
				if (i_x == j_x && j_x == k_x &&
				    (i_x == 0 || i_x == BOARD_WIDTH - 1) ||
				    i_y == j_y && j_y == k_y &&
				    (i_y == 0 || i_y == BOARD_HEIGHT - 1))
				{
					dead_pattern_table[BORDER][i][j][k] = 1;
					dead_pattern_table[BORDER][i][k][j] = 1;
					dead_pattern_table[BORDER][j][i][k] = 1;
					dead_pattern_table[BORDER][j][k][i] = 1;
					dead_pattern_table[BORDER][k][i][j] = 1;
					dead_pattern_table[BORDER][k][j][i] = 1;
				}
			}
		}
	}

	/* HORIZONTAL */
	for (i = 0; i < SQUARES; ++i) {
		i_x = i % 5;
		i_y = i / 5;
		for (j = i + 1; j < SQUARES; ++j) {
			j_x = j % 5;
			j_y = j / 5;
			for (k = j + 1; k < SQUARES; ++k) {
				k_x = k % 5;
				k_y = k / 5;
				if (i_x == j_x && j_x == k_x) {
					dead_pattern_table[HORIZONTAL][i][j][k] = 1;
					dead_pattern_table[HORIZONTAL][i][k][j] = 1;
					dead_pattern_table[HORIZONTAL][j][i][k] = 1;
					dead_pattern_table[HORIZONTAL][j][k][i] = 1;
					dead_pattern_table[HORIZONTAL][k][i][j] = 1;
					dead_pattern_table[HORIZONTAL][k][j][i] = 1;
				}
			}
		}
	}
}
