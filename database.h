#ifndef DATABASE_H
#define DATABASE_H

#include "const.h"
#include "ruleset.h"
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

int result_ok(result_t result);

enum player result_get_winner(result_t result);
result_t result_set_winner(result_t result, enum player player);

int result_get_depth(result_t result);
result_t result_set_depth(result_t result, int depth);

result_t max_result(enum player player, result_t r0, result_t r1);

/* Returns the number of characters written on success, -1 if the
 * result is invalid, and -2 if the buffer is too short
 */
int show_result(result_t result, char *string, int N);

void generate_database(enum ruleset ruleset);

result_t lookup(struct position *position);

#endif
