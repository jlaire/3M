#ifndef MOVE_H
#define MOVE_H

/* Only the lowest 2 bytes are used
 * 8b:from, 8b:to
 */
typedef int move_t;

#define MOVE_FROM_MASK 0xff00
#define MOVE_TO_MASK   0xff

int move_get_from(move_t move);
int move_get_to(move_t move);
move_t move_set_from(move_t move, int from);
move_t move_set_to(move_t move, int to);

/* Checks that from and to are in bounds and adjacent */
int move_valid(move_t move);

/* Returns the number of characters read on success, -1 on failure */
int read_move(move_t *move, char *string);

/* Returns the number of characters written on success, -1 if the
 * move is invalid, and -2 if the buffer is too short
 */
int show_move(move_t move, char *string, int N);

#endif
