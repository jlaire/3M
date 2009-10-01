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

int musketeers_to_index[SQUARES][SQUARES][SQUARES];
int index_to_musketeers[MUSKETEER_INDICES][3];

void init_indexing_tables(void) {
	int i;
	int j;
	int k;
	int count = 0;

	init_factorial();
	init_combinations();
	init_enemy_indices();
	init_indices();

	for (i = 0; i < SQUARES; ++i) {
		for (j = i + 1; j < SQUARES; ++j) {
			for (k = j + 1; k < SQUARES; ++k) {
				musketeers_to_index[i][j][k] = count;
				musketeers_to_index[i][k][j] = count;
				musketeers_to_index[j][i][k] = count;
				musketeers_to_index[j][k][i] = count;
				musketeers_to_index[k][i][j] = count;
				musketeers_to_index[k][j][i] = count;
				index_to_musketeers[count][0] = i;
				index_to_musketeers[count][1] = j;
				index_to_musketeers[count][2] = k;
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

uint64_t position_to_index(struct position *position) {
	uint64_t index = 0;
	int ms[3];
	int musketeers = 0;
	int i;
	int j;
	int p[MAX_ENEMIES];
	int enemies = count_enemies(position);

	j = 0;
	for (i = 0; i < SQUARES; ++i) {
		switch (position->squares[i]) {
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

	if (musketeers != 3)
		return -1;

	index *= indices[enemies];
	index += combination_to_index(p, MAX_ENEMIES, enemies);

	index *= MUSKETEER_INDICES;
	index += musketeers_to_index[ms[0]][ms[1]][ms[2]];

	return index;
}

void index_to_position(struct position *position, int enemies, uint64_t index) {
	int musketeer_index;
	int p[MAX_ENEMIES];
	int i;
	int j;

	for (i = 0; i < SQUARES; ++i)
		position->squares[i] = EMPTY;

	musketeer_index = index % MUSKETEER_INDICES;
	index /= MUSKETEER_INDICES;

	position->squares[index_to_musketeers[musketeer_index][0]] = MUSKETEER;
	position->squares[index_to_musketeers[musketeer_index][1]] = MUSKETEER;
	position->squares[index_to_musketeers[musketeer_index][2]] = MUSKETEER;

	index_to_combination(p, index, MAX_ENEMIES, enemies);

	for (i = j = 0; i < SQUARES; ++i)
		if (position->squares[i] != MUSKETEER)
			position->squares[i] = p[j++] == 0 ? EMPTY : ENEMY;
}
