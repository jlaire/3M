#ifndef POSITION_H
#define POSITION_H

#include <stdint.h>

#include "const.h"
#include "move.h"
#include "ruleset.h"

/* 10b:blank
 *  2b:ruleset
 *  1b:error?
 *  1b:turn (0 = MUSKETEERS, 1 = ENEMIES)
 * 25b:MUSKETEER?
 * 25b:ENEMY?
 */
typedef uint64_t position_t;

#define RULESET_OFFSET   52L
#define ERROR_OFFSET     51L
#define TURN_OFFSET      50L
#define MUSKETEER_OFFSET 25L
#define ENEMY_OFFSET     0L

#define RULESET_MASK     (((1L << 2L) - 1L) << RULESET_OFFSET)
#define ERROR_BIT        (1L << ERROR_OFFSET)
#define TURN_BIT         (1L << TURN_OFFSET)
#define MUSKETEER_MASK   (((1L << 25L) - 1L) << MUSKETEER_OFFSET)
#define ENEMY_MASK       (((1L << 25L) - 1L) << ENEMY_OFFSET)

#define MUSKETEER_BIT(i) ((1L << MUSKETEER_OFFSET + 24L) >> (i))
#define ENEMY_BIT(i)     ((1L << ENEMY_OFFSET + 24L) >> (i))

inline enum ruleset get_ruleset(position_t position);
inline position_t set_ruleset(position_t position, enum ruleset ruleset);

inline enum player get_turn(position_t position);
inline position_t set_turn(position_t position, enum player player);

inline position_t put_musketeer(position_t position, square_t i);
inline position_t remove_musketeer(position_t position, square_t i);
inline position_t put_enemy(position_t position, square_t i);
inline position_t remove_enemy(position_t position, square_t i);
inline position_t set_empty(position_t position, square_t i);

inline enum square get_square(position_t position, square_t i);
inline position_t set_square(position_t position, square_t i, enum square square);

inline uint64_t is_musketeer(position_t position, square_t i);
inline uint64_t is_enemy(position_t position, square_t i);
inline uint64_t is_empty(position_t position, square_t i);

inline uint64_t position_ok(position_t position);

extern position_t start_position;
extern position_t error_position;

void init_positions(void);

inline unsigned int count_musketeers(position_t position);
inline unsigned int count_enemies(position_t position);

/* Returns the number of characters read on success, -1 on failure */
int read_position(position_t *position, char *string);

/* Returns the number of characters written on success, -1 if the
 * position is invalid, and -2 if the buffer is too short
 */
int show_position(position_t position, char *string, int N);

/* These assume that the move/undo is valid and game isn't over */
int move_legal(position_t position, move_t move);
int undo_legal(position_t position, move_t undo);

/* These don't */
int move_valid_and_legal(position_t position, move_t move);
int undo_valid_and_legal(position_t position, move_t undo);

position_t apply_move(position_t position, move_t move);
position_t apply_undo(position_t position, move_t undo);

/* Return value is the number of moves/undos */
int list_legal_moves(position_t position, move_t *moves);
int list_legal_undos(position_t position, move_t *undos);

/* If the game is over, returns MUSKETEERS or ENEMIES -- if not, NOBODY */
enum player winner(position_t position);

inline enum ruleset get_ruleset(position_t position) {
	return (position & RULESET_MASK) >> RULESET_OFFSET;
}

inline position_t set_ruleset(position_t position, enum ruleset ruleset) {
	return position & ~RULESET_MASK | (uint64_t)ruleset << RULESET_OFFSET;
}

inline enum player get_turn(position_t position) {
	return (position & TURN_BIT) >> TURN_OFFSET;
}

inline position_t set_turn(position_t position, enum player player) {
	return position & ~TURN_BIT | (uint64_t)player << TURN_OFFSET;
}

inline position_t put_musketeer(position_t position, square_t i) {
	return position | MUSKETEER_BIT(i);
}

inline position_t remove_musketeer(position_t position, square_t i) {
	return position & ~MUSKETEER_BIT(i);
}

inline position_t put_enemy(position_t position, square_t i) {
	return position | ENEMY_BIT(i);
}

inline position_t remove_enemy(position_t position, square_t i) {
	return position & ~ENEMY_BIT(i);
}

inline position_t set_empty(position_t position, square_t i) {
	return remove_musketeer(remove_enemy(position, i), i);
}

inline enum square get_square(position_t position, square_t i) {
	return is_musketeer(position, i) ? MUSKETEER :
	       is_enemy(position, i) ? ENEMY :
	       EMPTY;
}

inline position_t set_square(position_t position, square_t i, enum square square) {
	position = set_empty(position, i);
	switch (square) {
		case MUSKETEER: return put_musketeer(position, i);
		case ENEMY: return put_enemy(position, i);
		default: return position;
	}
}

inline uint64_t is_musketeer(position_t position, square_t i) {
	return position & MUSKETEER_BIT(i);
}

inline uint64_t is_enemy(position_t position, square_t i) {
	return position & ENEMY_BIT(i);
}

inline uint64_t is_empty(position_t position, square_t i) {
	return !is_musketeer(position, i) && !is_enemy(position, i);
}

inline uint64_t position_ok(position_t position) {
	return !(position & ERROR_BIT);
}

inline unsigned int count_musketeers(position_t position) {
	return count_bits_25((position & MUSKETEER_MASK) >> MUSKETEER_OFFSET);
}

inline unsigned int count_enemies(position_t position) {
	return count_bits_25((position & ENEMY_MASK) >> ENEMY_OFFSET);
}

#endif
