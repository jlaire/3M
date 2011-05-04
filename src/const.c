#include <math.h>
#include <stdint.h>

#include "const.h"

int adjacent[SQUARES][SQUARES] = {{0}};

void init_adjacent(void) {
	for (int i = 0; i < SQUARES; ++i) {
		int i_x = i % 5;
		int i_y = i / 5;
		for (int j = 0; j < SQUARES; ++j) {
			int j_x = j % 5;
			int j_y = j / 5;
			if (i_x == j_x && fabs(i_y - j_y) == 1 ||
			    i_y == j_y && fabs(i_x - j_x) == 1)
			{
				adjacent[i][j] = 1;
				adjacent[j][i] = 1;
			}
		}
	}
}

int neighbours[SQUARES][MAX_NEIGHBOURS];

void init_neighbours(void) {
	for (int i = 0; i < SQUARES; ++i) {
		int k = 0;
		for (int j = 0; j < SQUARES; ++j)
			if (adjacent[i][j])
				neighbours[i][k++] = j;
		for (; k < MAX_NEIGHBOURS; ++k)
			neighbours[i][k] = -1;
	}
}

unsigned int popcnt(unsigned int n) {
	unsigned int c;
	for (c = 0; n; ++c)
		n &= n - 1;
	return c;
}

uint8_t dead_pattern_table[1 << SQUARES] = {0};

void init_dead_pattern_table(void) {
	for (int i = 0; i < SQUARES; ++i) {
		int i_x = i % 5;
		int i_y = i / 5;
		for (int j = i + 1; j < SQUARES; ++j) {
			int j_x = j % 5;
			int j_y = j / 5;
			for (int k = j + 1; k < SQUARES; ++k) {
				int k_x = k % 5;
				int k_y = k / 5;
				int n = 1 << (SQUARES - 1 - i)
				      | 1 << (SQUARES - 1 - j)
				      | 1 << (SQUARES - 1 - k);
				if (i_x == j_x && j_x == k_x ||
				    i_y == j_y && j_y == k_y)
				{
					dead_pattern_table[n] = 1;
				}
			}
		}
	}
}
