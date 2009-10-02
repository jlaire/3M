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

enum ruleset get_ruleset(position_t position);
position_t set_ruleset(position_t position, enum ruleset ruleset);

enum player get_turn(position_t position);
position_t set_turn(position_t position, enum player player);

position_t put_musketeer(position_t position, int i);
position_t remove_musketeer(position_t position, int i);
position_t put_enemy(position_t position, int i);
position_t remove_enemy(position_t position, int i);
position_t set_empty(position_t position, int i);

enum square get_square(position_t position, int i);
position_t set_square(position_t position, int i, enum square square);

uint64_t is_musketeer(position_t position, int square);
uint64_t is_enemy(position_t position, int square);
uint64_t is_empty(position_t position, int square);

extern position_t start_position;
extern position_t error_position;

void init_positions(void);

uint64_t position_ok(position_t position);

int count_musketeers(position_t position);
int count_enemies(position_t position);

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
int move_legal_safe(position_t position, move_t move);
int undo_legal_safe(position_t position, move_t undo);

position_t apply_move(position_t position, move_t move);
position_t apply_undo(position_t position, move_t undo);

/* Return value is the number of moves/undos */
int list_legal_moves(position_t position, move_t *moves);
int list_legal_undos(position_t position, move_t *undos);

/* If the game is over, returns MUSKETEERS or ENEMIES -- if not, NOBODY */
enum player winner(position_t position);

#endif
