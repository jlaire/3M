#ifndef DATABASE_H
#define DATABASE_H

#include <stdint.h>

#include "const.h"
#include "position.h"

/* 1b: 1 = error, 0 = ok
 * 1b: winner (0 = MUSKETEERS, 1 = ENEMIES)
 * 6b: depth (unsigned integer)
 */
typedef uint8_t result_t;

extern const result_t error_result;
extern const result_t ok_result;

#define RESULT_ERROR_BIT (1 << 7)
#define RESULT_WINNER_BIT (1 << 6)
#define RESULT_DEPTH_MASK ((1 << 6) - 1)

inline int result_ok(result_t result);

inline enum player result_get_winner(result_t result);
inline result_t result_set_winner(result_t result, enum player player);

inline int result_get_depth(result_t result);
inline result_t result_set_depth(result_t result, int depth);

/* Returns r0 or r1, whichever is better for `player'.
 * If they are equally good, r0 is returned.
 * - Any result is better than error_result
 * - Winning is better than losing
 * - Winning in N moves is better than winning in K moves iff N < K
 * - Losing in N moves is better than losing in K moves iff N > K
 */
result_t max_result(enum player player, result_t r0, result_t r1);

/* Writes a short English sentence describing `result' to `string'. Returns the
 * number of characters written on success, -1 if the result is invalid, and -2
 * if the buffer is too short.
 */
int show_result(result_t result, char *string, int N);

/* Displays `result' as [ME]\d+, indicating the winning player and number of
 * moves required to force the win. Returns the number of characters written on
 * success, -2 if the buffer is too short.
 */
int show_result_short(result_t result, char *string, int N);

/* Set path to the data folder.
 */
void set_database_path(const char *path);

/* Get path to the data folder.
 */
const char *get_database_path(void);

/* Generates all files required to play every position optimally.
 * Returns 0 on success, -1 on failure. */
int generate_database(void);

/* Generates all files required for lookup() to work on this `position'.
 * Returns 0 on success, -1 on failure. */
int solve(position_t position);

/* Returns error_result on error. */
result_t lookup(position_t position);

/********************/
/* inline functions */
/********************/

inline int result_ok(result_t result) {
	return (result & RESULT_ERROR_BIT) == 0;
}

inline enum player result_get_winner(result_t result) {
	return result & RESULT_WINNER_BIT ? ENEMIES : MUSKETEERS;
}

inline result_t result_set_winner(result_t result, enum player player) {
	if (player == MUSKETEERS)
		return result & ~RESULT_WINNER_BIT;
	else if (player == ENEMIES)
		return result | RESULT_WINNER_BIT;
	else
		return error_result;
}

inline int result_get_depth(result_t result) {
	return result & RESULT_DEPTH_MASK;
}

inline result_t result_set_depth(result_t result, int depth) {
	return (result & ~RESULT_DEPTH_MASK) | (depth & RESULT_DEPTH_MASK);
}

inline result_t result_inc_depth(result_t result) {
	return result + 1;
}

#endif
