#include <stdint.h>

#include "const.h"
#include "ruleset.h"
#include "position.h"
#include "indexing.h"

#define MAX_FACTORIAL 20
uint64_t factorial[MAX_FACTORIAL + 1];

void init_factorial(void) {
	int i;

	factorial[0] = 1;
	for (i = 1; i <= MAX_FACTORIAL; ++i)
		factorial[i] = factorial[i - 1] * i;
}

#define MAX_COMB 50
uint64_t combinations[MAX_COMB + 1][MAX_COMB + 1] = {{0}};

void init_combinations(void) {
	uint64_t n;
	uint64_t r;

	for (n = 0; n <= MAX_COMB; ++n)
		combinations[n][0] = 1;

	for (n = 1; n <= MAX_COMB; ++n) {
		for (r = 1; r <= n; ++r) {
			combinations[n][r] =
				combinations[n - 1][r - 1] +
				combinations[n - 1][r];
		}
	}
}

uint64_t enemy_indices[MAX_ENEMIES + 1];

void init_enemy_indices(void) {
	int i;

	for (i = 0; i <= MAX_ENEMIES; ++i)
		enemy_indices[i] = combinations[MAX_ENEMIES][i];
}

uint64_t indices[MAX_ENEMIES + 1];

void init_indices(void) {
	int i;

	for (i = 0; i <= MAX_ENEMIES; ++i)
		indices[i] = MUSKETEER_INDICES * enemy_indices[i];
}

uint16_t musketeers_to_index[1 << 25];
uint64_t index_to_musketeers[MUSKETEER_INDICES];

void init_indexing_tables(void) {
	uint64_t i;
	uint64_t j;
	uint64_t k;
	uint64_t n;
	uint64_t count = 0;

	init_factorial();
	init_combinations();
	init_enemy_indices();
	init_indices();

	for (i = 0; i < SQUARES; ++i) {
		for (j = i + 1; j < SQUARES; ++j) {
			for (k = j + 1; k < SQUARES; ++k) {
				n = 1 << 24 - i | 1 << 24 - j | 1 << (24 - k);
				musketeers_to_index[n] = count;
				index_to_musketeers[count] = n;
				++count;
			}
		}
	}
}

uint64_t combination_to_index(int *p, int N, int K) {
	uint64_t index = 0;
	int i;
	int r = K;

	for (i = 0; i < N; ++i)
		if (p[i])
			index += combinations[N - i - 1][r--];

	return index;
}

void index_to_combination(int *p, uint64_t index, int N, int K) {
	int i;
	int r = K;

	for (i = 0; i < N; ++i) {
		if (index >= combinations[N - i - 1][r]) {
			p[i] = 1;
			index -= combinations[N - i - 1][r];
			--r;
		}
		else
			p[i] = 0;
	}
}

uint64_t position_to_index(position_t position) {
	uint64_t index;
	int ms[3];
	int musketeers = 0;
	int i;
	int j;
	int n;
	int p[MAX_ENEMIES];
	int enemies = count_enemies(position);

	if (count_musketeers(position) != 3)
		return -1;

	j = 0;
	for (i = 0; i < SQUARES; ++i) {
		switch (get_square(position, i)) {
		case MUSKETEER:
			ms[musketeers++] = i;
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

	index = combination_to_index(p, MAX_ENEMIES, enemies);

	index *= MUSKETEER_INDICES;
	n = 1 << (24 - ms[0]) | 1 << (24 - ms[1]) | 1 << (24 - ms[2]);
	index += musketeers_to_index[n];

	return index;
}

position_t index_to_position(position_t position, int enemies, uint64_t index) {
	int musketeer_index;
	int p[MAX_ENEMIES];
	int i;
	int j;

	position &= ~((1L << 50L) - 1L);

	musketeer_index = index % MUSKETEER_INDICES;
	index /= MUSKETEER_INDICES;

	position |= index_to_musketeers[musketeer_index] << MUSKETEER_OFFSET;

	index_to_combination(p, index, MAX_ENEMIES, enemies);

	for (i = j = 0; i < SQUARES; ++i)
		if (!is_musketeer(position, i) && p[j++] == 1)
			position = put_enemy(position, i);

	return position;
}
