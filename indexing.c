#include <stdint.h>

#include "const.h"
#include "ruleset.h"
#include "position.h"
#include "indexing.h"

uint64_t translate(const int *p, uint64_t in) {
	uint64_t out = 0;

	for (int i = 0; i < 25; ++i)
		if (in & 1 << 24 - i)
			out |= 1 << 24 - p[i];

	return out;
}

position_t translate_position(const int *p, position_t pos) {
	uint64_t new_es = translate(p, (pos & ENEMY_MASK) >> ENEMY_OFFSET);
	uint64_t new_ms = translate(p, (pos & MUSKETEER_MASK) >> MUSKETEER_OFFSET);

	pos &= ~ENEMY_MASK;
	pos &= ~MUSKETEER_MASK;

	pos |= new_es << ENEMY_OFFSET;
	pos |= new_ms << MUSKETEER_OFFSET;

	return pos;
}

const int rotate_cw[] = {
	 4,  9, 14, 19, 24,
	 3,  8, 13, 18, 23,
	 2,  7, 12, 17, 22,
	 1,  6, 11, 16, 21,
	 0,  5, 10, 15, 20,
};

const int reflect_h[] = {
	20, 21, 22, 23, 24,
	15, 16, 17, 18, 19,
	10, 11, 12, 13, 14,
	 5,  6,  7,  8,  9,
	 0,  1,  2,  3,  4,
};

uint64_t minimum_25b(enum ruleset rs, uint64_t n) {
	uint64_t min = n;

	for (int rot = 0; rot < 4; ++rot) {
		n = translate(rotate_cw, n);
		if (rs == HORIZONTAL && rot % 2 == 0)
			continue;

		if (n < min)
			min = n;

		uint64_t temp = translate(reflect_h, n);
		if (temp < min)
			min = temp;
	}

	return min;
}

uint64_t musketeer_indices[RULESETS];

uint16_t musketeers_to_index[RULESETS][1 << 25];
uint64_t index_to_musketeers[RULESETS][MAX_MUSKETEER_INDICES];

inline int position_normal(position_t pos) {
	enum ruleset rs = get_ruleset(pos);
	uint64_t musketeers = (pos & MUSKETEER_MASK) >> MUSKETEER_OFFSET;
	return musketeers_to_index[rs][musketeers] != 0;
}

position_t normalize_position(position_t pos) {
	enum ruleset rs = get_ruleset(pos);

	for (int rot = 0; rot < 4; ++rot) {
		pos = translate_position(rotate_cw, pos);
		if (rs == HORIZONTAL && rot % 2 == 0)
			continue;

		if (position_normal(pos))
			return pos;

		position_t temp = translate_position(reflect_h, pos);
		if (position_normal(temp))
			return temp;
	}

	return error_position;
}

void init_musketeer_indexing(void) {
	for (enum ruleset rs = 0; rs < RULESETS; ++rs) {
		int16_t count = 0;
		for (uint64_t i = 0; i < SQUARES; ++i) {
			for (uint64_t j = i + 1; j < SQUARES; ++j) {
				for (uint64_t k = j + 1; k < SQUARES; ++k) {
					uint64_t n = 1 << 24 - i |
					             1 << 24 - j |
					             1 << 24 - k;
					n = minimum_25b(rs, n);
					if (musketeers_to_index[rs][n] != 0)
						continue;
					musketeers_to_index[rs][n] = count + 1;
					index_to_musketeers[rs][count] = n;
					++count;
				}
			}
		}
		musketeer_indices[rs] = count;
	}
}

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

uint64_t indices[RULESETS][MAX_ENEMIES + 1];

void init_indices(void) {
	for (enum ruleset rs = 0; rs < RULESETS; ++rs)
		for (int i = 0; i <= MAX_ENEMIES; ++i)
			indices[rs][i] = musketeer_indices[rs] * enemy_indices[i];
}

void init_indexing_tables(void) {
	init_musketeer_indexing();
	init_choose();
	init_enemy_indices();
	init_indices();
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

	position = normalize_position(position);

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
	enum ruleset rs = get_ruleset(position);
	index *= musketeer_indices[rs];
	index += musketeers_to_index[rs][n] - 1;

	return index;
}

position_t index_to_position(position_t position, int enemies, uint64_t index) {
	enum ruleset rs = get_ruleset(position);
	position &= ~((1L << 50L) - 1L);

	int musketeer_index = index % musketeer_indices[rs];
	index /= musketeer_indices[rs];

	position |= index_to_musketeers[rs][musketeer_index] << MUSKETEER_OFFSET;

	int p[MAX_ENEMIES];
	index_to_combination(p, index, MAX_ENEMIES, enemies);

	for (int i = 0, j = 0; i < SQUARES; ++i)
		if (!is_musketeer(position, i) && p[j++] == 1)
			position = put_enemy(position, i);

	return position;
}
