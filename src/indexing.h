#ifndef INDEXING_H
#define INDEXING_H

#include <stdint.h>

#include "position.h"

/* Parameter p is assumed to point to N boolean values exactly K of which are
 * non-zero. The returned index is in the range [0, choose(N, K)).
 */
uint64_t combination_to_index(const int *p, unsigned int N, unsigned int K);

/* Inverse of combination_to_index() */
void index_to_combination(int *p, uint64_t index, unsigned int N, unsigned int K);

extern uint64_t musketeer_indices;

/* enemy_indices[n] = choose(MAX_ENEMIES, n) */
extern uint64_t enemy_indices[MAX_ENEMIES + 1];

/* indices[n] = musketeer_indices * enemy_indices[n] */
extern uint64_t indices[MAX_ENEMIES + 1];

/* These functions implement a 1-to-1 mapping from positions to integers in the
 * range [0, indices[enemies]). Only the pieces are stored; the turn is lost.
 * index_to_position leaves the turn unmodified
 */
uint64_t position_to_index(position_t position);
position_t index_to_position(position_t position, int enemies, uint64_t index);

void init_indexing_tables(void);

position_t normalize_position(position_t position);

#endif
