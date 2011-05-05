#include <assert.h>
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

// C(WIDTH, MUSKETEERS) * HEIGHT + C(HEIGHT, MUSKETEERS) * WIDTH
// C(5,3) * 5 + C(5,3) * 5
// C(5,3) * 10
// 10 * 10
#define DEAD_PATTERNS 100
static inline void init_dead_patterns(uint32_t *deads) {
	int count = 0;
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
					deads[count++] = n;
				}
			}
		}
	}

	assert(count == DEAD_PATTERNS);

	// insertion sort
	for (int i = 1; i < count; ++i) {
		for (int j = i - 1; j >= 0 && deads[j] > deads[j + 1]; --j) {
			int temp = deads[j];
			deads[j] = deads[j + 1];
			deads[j + 1] = temp;
		}
	}
}

int is_dead_pattern(uint32_t musketeers) {
	static uint32_t deads[DEAD_PATTERNS];
	static int init_done;
	if (!init_done) {
		init_dead_patterns(deads);
		init_done = 1;
	}
	int lo = 0;
	int hi = DEAD_PATTERNS - 1;
	while (lo < hi) {
		int mid = lo + (hi - lo) / 2;
		if (deads[mid] < musketeers) {
			lo = mid + 1;
		}
		else if (deads[mid] > musketeers) {
			hi = mid - 1;
		}
		else {
			return 1;
		}
	}
	return musketeers == deads[lo];
}
