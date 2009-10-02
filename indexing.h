#ifndef INDEXING_H
#define INDEXING_H

#include <stdint.h>

#include "position.h"

/* Parameter p is assumed to point to N truth values exactly K
 * of which are true. The index is in the range [0, choose(N, K)).
 */
uint64_t combination_to_index(int *p, int N, int K);
void index_to_combination(int *p, uint64_t index, int N, int K);

/* choose(25, 3) */
#define MUSKETEER_INDICES 2300L

/* enemy_indices[n] = choose(MAX_ENEMIES, n) */
extern uint64_t enemy_indices[MAX_ENEMIES + 1];

/* indices[n] = enemy_indices[n] * MUSKETEER_INDICES */
extern uint64_t indices[MAX_ENEMIES + 1];

/* These functions implement a 1-to-1 mapping from positions to
 * integers in the range [0, indices[enemies]). Only the locations of
 * the pieces are stored; the ruleset and turn are lost.
 * index_to_position leaves them unmodified
 */
uint64_t position_to_index(position_t position);
position_t index_to_position(position_t position, int enemies, uint64_t index);

void init_indexing_tables(void);

#endif
