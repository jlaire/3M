#include <stdint.h>

#include "const.h"
#include "ruleset.h"
#include "position.h"
#include "indexing.h"

#define MAX_COMB 50
uint64_t choose[MAX_COMB + 1][MAX_COMB + 1] = {{0}};

void init_choose(void) {
	for (uint64_t n = 0; n <= MAX_COMB; ++n)
		choose[n][0] = 1;

	for (uint64_t n = 1; n <= MAX_COMB; ++n)
		for (uint64_t r = 1; r <= n; ++r)
			choose[n][r] = choose[n - 1][r - 1] + choose[n - 1][r];
}

uint64_t enemy_indices[MAX_ENEMIES + 1];

void init_enemy_indices(void) {
	for (int i = 0; i <= MAX_ENEMIES; ++i)
		enemy_indices[i] = choose[MAX_ENEMIES][i];
}

uint64_t indices[MAX_ENEMIES + 1];

void init_indices(void) {
	for (int i = 0; i <= MAX_ENEMIES; ++i)
		indices[i] = MUSKETEER_INDICES * enemy_indices[i];
}

uint16_t musketeers_to_index[1 << 25];
uint64_t index_to_musketeers[MUSKETEER_INDICES];

void init_indexing_tables(void) {
	init_choose();
	init_enemy_indices();
	init_indices();

	uint64_t count = 0;

	for (uint64_t i = 0; i < SQUARES; ++i) {
		for (uint64_t j = i + 1; j < SQUARES; ++j) {
			for (uint64_t k = j + 1; k < SQUARES; ++k) {
				uint64_t n = 1 << 24 - i |
				             1 << 24 - j |
				             1 << 24 - k;
				musketeers_to_index[n] = count;
				index_to_musketeers[count] = n;
				++count;
			}
		}
	}
}

uint64_t combination_to_index(const int *p, unsigned int N, unsigned int K) {
	uint64_t index = 0;
	unsigned int r = K;

	for (square_t i = 0; i < N; ++i)
		if (p[i])
			index += choose[N - i - 1][r--];

	return index;
}

void index_to_combination(int *p, uint64_t index, unsigned int N, unsigned int K) {
	unsigned int r = K;

	for (square_t i = 0; i < N; ++i) {
		if (index >= choose[N - i - 1][r]) {
			p[i] = 1;
			index -= choose[N - i - 1][r];
			--r;
		}
		else
			p[i] = 0;
	}
}

uint64_t position_to_index(position_t position) {
	if (count_musketeers(position) != 3)
		return -1;

	int musketeers[MUSKETEER_COUNT];
	int p[MAX_ENEMIES];

	int j = 0;
	for (int i = 0, m = 0; i < SQUARES; ++i) {
		switch (get_square(position, i)) {
		case MUSKETEER:
			musketeers[m++] = i;
			break;
		case EMPTY:
			p[j++] = 0;
			break;
		case ENEMY:
			p[j++] = 1;
			break;
		default:
			return -1;
		}
	}

	int enemies = count_enemies(position);
	uint64_t index = combination_to_index(p, MAX_ENEMIES, enemies);

	int n = 1 << 24 - musketeers[0] |
	        1 << 24 - musketeers[1] |
	        1 << 24 - musketeers[2];
	index *= MUSKETEER_INDICES;
	index += musketeers_to_index[n];

	return index;
}

position_t index_to_position(position_t position, int enemies, uint64_t index) {
	position &= ~((1L << 50L) - 1L);

	int musketeer_index = index % MUSKETEER_INDICES;
	index /= MUSKETEER_INDICES;

	position |= index_to_musketeers[musketeer_index] << MUSKETEER_OFFSET;

	int p[MAX_ENEMIES];
	index_to_combination(p, index, MAX_ENEMIES, enemies);

	for (int i = 0, j = 0; i < SQUARES; ++i)
		if (!is_musketeer(position, i) && p[j++] == 1)
			position = put_enemy(position, i);

	return position;
}
