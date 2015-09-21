#include <stdint.h>

#include "const.h"
#include "position.h"
#include "indexing.h"

static uint64_t translate(const int *p, uint64_t in) {
	uint64_t out = 0;

	for (int i = 0; i < 25; ++i)
		if (in & 1 << (24 - i))
			out |= 1 << (24 - p[i]);

	return out;
}

static position_t translate_position(const int *p, position_t pos) {
	uint64_t new_es = translate(p, (pos & ENEMY_MASK) >> ENEMY_OFFSET);
	uint64_t new_ms = translate(p, (pos & MUSKETEER_MASK) >> MUSKETEER_OFFSET);

	pos &= ~ENEMY_MASK;
	pos &= ~MUSKETEER_MASK;

	pos |= new_es << ENEMY_OFFSET;
	pos |= new_ms << MUSKETEER_OFFSET;

	return pos;
}

static const int rotate_cw[] = {
	 4,  9, 14, 19, 24,
	 3,  8, 13, 18, 23,
	 2,  7, 12, 17, 22,
	 1,  6, 11, 16, 21,
	 0,  5, 10, 15, 20,
};

static const int reflect_h[] = {
	20, 21, 22, 23, 24,
	15, 16, 17, 18, 19,
	10, 11, 12, 13, 14,
	 5,  6,  7,  8,  9,
	 0,  1,  2,  3,  4,
};

static uint64_t minimum_25b(uint64_t n) {
	uint64_t min = n;

	for (int rot = 0; rot < 4; ++rot) {
		n = translate(rotate_cw, n);
		if (n < min)
			min = n;

		uint64_t temp = translate(reflect_h, n);
		if (temp < min)
			min = temp;
	}

	return min;
}

#define MAX_MUSKETEER_INDICES 2300 // C(25,3)
// The actual value is smaller than 2300 because of symmetries
static uint64_t musketeer_indices;

static uint16_t musketeers_to_index[1 << SQUARES];
static uint64_t index_to_musketeers[MAX_MUSKETEER_INDICES];

static inline int position_normal(position_t pos) {
	uint64_t musketeers = (pos & MUSKETEER_MASK) >> MUSKETEER_OFFSET;
	return musketeers_to_index[musketeers] != 0;
}

position_t normalize_position(position_t pos) {
	for (int rot = 0; rot < 4; ++rot) {
		pos = translate_position(rotate_cw, pos);
		if (position_normal(pos))
			return pos;

		position_t temp = translate_position(reflect_h, pos);
		if (position_normal(temp))
			return temp;
	}

	return error_position;
}

static void init_musketeer_indexing(void) {
	int16_t count = 0;
	for (uint64_t i = 0; i < SQUARES; ++i) {
		for (uint64_t j = i + 1; j < SQUARES; ++j) {
			for (uint64_t k = j + 1; k < SQUARES; ++k) {
				uint64_t n = 1 << (SQUARES - 1 - i)
					   | 1 << (SQUARES - 1 - j)
					   | 1 << (SQUARES - 1 - k);
				n = minimum_25b(n);
				if (musketeers_to_index[n] != 0)
					continue;
				musketeers_to_index[n] = count + 1;
				index_to_musketeers[count] = n;
				++count;
			}
		}
	}
	musketeer_indices = count;
}

#define MAX_COMB 50
static uint64_t choose[MAX_COMB + 1][MAX_COMB + 1] = {{0}};

static void init_choose(void) {
	for (uint64_t n = 0; n <= MAX_COMB; ++n)
		choose[n][0] = 1;

	for (uint64_t n = 1; n <= MAX_COMB; ++n)
		for (uint64_t r = 1; r <= n; ++r)
			choose[n][r] = choose[n - 1][r - 1] + choose[n - 1][r];
}

/* enemy_indices[n] = choose(MAX_ENEMIES, n) */
static uint64_t enemy_indices[MAX_ENEMIES + 1];

static void init_enemy_indices(void) {
	for (int i = 0; i <= MAX_ENEMIES; ++i)
		enemy_indices[i] = choose[MAX_ENEMIES][i];
}

uint64_t indices[MAX_ENEMIES + 1];

static void init_indices(void) {
	for (int i = 0; i <= MAX_ENEMIES; ++i)
		indices[i] = musketeer_indices * enemy_indices[i];
}

void init_indexing_tables(void) {
	init_musketeer_indexing();
	init_choose();
	init_enemy_indices();
	init_indices();
}

/* Parameter `p' must point to N boolean values exactly K of which are non-zero.
 * Return value is in [0, choose(N, K)).
 */
static uint64_t combination_to_index(const int *p, unsigned int N, unsigned int K) {
	uint64_t index = 0;
	unsigned int r = K;

	for (square_t i = 0; i < N; ++i)
		if (p[i])
			index += choose[N - i - 1][r--];

	return index;
}

static void index_to_combination(int *p, uint64_t index, unsigned int N, unsigned int K) {
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

	int n = 1 << (24 - musketeers[0]) |
	        1 << (24 - musketeers[1]) |
	        1 << (24 - musketeers[2]);
	index *= musketeer_indices;
	index += musketeers_to_index[n] - 1;

	return index;
}

position_t index_to_position(position_t position, int enemies, uint64_t index) {
	position &= ~((1L << 50L) - 1L);

	int musketeer_index = index % musketeer_indices;
	index /= musketeer_indices;

	position |= index_to_musketeers[musketeer_index] << MUSKETEER_OFFSET;

	int p[MAX_ENEMIES];
	index_to_combination(p, index, MAX_ENEMIES, enemies);

	for (int i = 0, j = 0; i < SQUARES; ++i)
		if (!is_musketeer(position, i) && p[j++] == 1)
			position = put_enemy(position, i);

	return position;
}
