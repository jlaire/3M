#ifndef MOVE_H
#define MOVE_H

struct move {
	int from;
	int to;
};

/* Checks that move.from and move.to are in bounds and adjacent */
int move_valid(struct move *move);

/* Returns the number of characters read on success, -1 on failure */
int read_move(struct move *move, char *string);

/* Returns the number of characters written on success, -1 if the
 * move is invalid, and -2 if the buffer is too short
 */
int show_move(struct move *move, char *string, int N);

#endif
