#ifndef INDEXING_H
#define INDEXING_H

#include <stdint.h>

#include "position.h"

/* indices[n] is the number of different positions with n enemies on board */
extern uint64_t indices[MAX_ENEMIES + 1];

/* These functions implement a 1-to-1 mapping from positions to integers in the
 * range [0, indices[enemies]). Only the pieces are stored in the encoded
 * integer value; the turn is lost.
 * index_to_position leaves the turn unmodified.
 */
uint64_t position_to_index(position_t position);
position_t index_to_position(position_t position, int enemies, uint64_t index);

void init_indexing_tables(void);

position_t normalize_position(position_t position);

#endif
