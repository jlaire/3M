#ifndef POSITION_H
#define POSITION_H

#include "const.h"
#include "move.h"
#include "ruleset.h"

struct position {
	enum ruleset ruleset;
	enum player player_to_move;
	enum square squares[SQUARES];
};

void set_to_start_position(struct position *position);

int count_musketeers(struct position *position);
int count_enemies(struct position *position);

/* Returns the number of characters read on success, -1 on failure */
int read_position(struct position *position, char *string);

/* Returns the number of characters written on success, -1 if the
 * position is invalid, and -2 if the buffer is too short
 */
int show_position(struct position *position, char *string, int N);

int move_legal(struct position *position, struct move *move);
int undo_legal(struct position *position, struct move *undo);

/* Return value is 0 on success and 1 on failure */
int apply_move(struct position *position, struct move *move);
int apply_undo(struct position *position, struct move *undo);

/* Return value is the number of moves/undos */
int list_legal_moves(struct position *position, struct move *moves);
int list_legal_undos(struct position *position, struct move *undos);

/* If the game is over, returns MUSKETEERS or ENEMIES -- if not, NOBODY */
enum player winner(struct position *position);

#endif
