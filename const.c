#include <math.h>
#include <stdint.h>

#include "const.h"
#include "ruleset.h"

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

uint8_t count_bits_25[1 << 25] = {0};

void init_count_bits_25(void) {
	int v;
	int c;
	int i;

	for (i = 0; i < 1 << 25; ++i) {
		for (c = 0, v = i; v; ++c)
			v &= v - 1;
		count_bits_25[i] = c;
	}
}

uint8_t dead_pattern_table[RULESETS][1 << 25] = {{0}};

void init_dead_pattern_table(void) {
	int i, i_x, i_y;
	int j, j_x, j_y;
	int k, k_x, k_y;
	int n;

	for (i = 0; i < SQUARES; ++i) {
		i_x = i % 5;
		i_y = i / 5;
		for (j = i + 1; j < SQUARES; ++j) {
			j_x = j % 5;
			j_y = j / 5;
			for (k = j + 1; k < SQUARES; ++k) {
				k_x = k % 5;
				k_y = k / 5;
				n = 1 << (24 - i) | 1 << (24 - j) | 1 << (24 - k);
				if (i_x == j_x && j_x == k_x ||
				    i_y == j_y && j_y == k_y)
				{
					dead_pattern_table[STANDARD][n] = 1;
				}
				if (i_x == j_x && j_x == k_x &&
				    (i_x == 0 || i_x == BOARD_WIDTH - 1) ||
				    i_y == j_y && j_y == k_y &&
				    (i_y == 0 || i_y == BOARD_HEIGHT - 1))
				{
					dead_pattern_table[BORDER][n] = 1;
				}
				if (i_x == j_x && j_x == k_x) {
					dead_pattern_table[HORIZONTAL][n] = 1;
				}
			}
		}
	}
}
