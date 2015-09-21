#ifndef MOVE_H
#define MOVE_H

/* Only the lowest 2 bytes are used
 * 8b:from, 8b:to
 */
typedef unsigned int move_t;

#define MOVE_FROM_OFFSET 8
#define MOVE_FROM_MASK   0xff00
#define MOVE_TO_MASK     0xff

inline square_t move_get_from(move_t move);
inline square_t move_get_to(move_t move);
inline move_t move_set_from(move_t move, square_t from);
inline move_t move_set_to(move_t move, square_t to);

/* Checks that from and to are in bounds and adjacent */
int move_valid(move_t move);

/* Returns the number of characters read on success, -1 on failure */
int read_move(move_t *move, const char *string);

/* Returns the number of characters written on success, -1 if the
 * move is invalid, and -2 if the buffer is too short
 */
int show_move(move_t move, char *string, int N);

inline square_t move_get_from(move_t move) {
	return move >> MOVE_FROM_OFFSET;
}

inline square_t move_get_to(move_t move) {
	return move & MOVE_TO_MASK;
}

inline move_t move_set_from(move_t move, square_t from) {
	return (move & MOVE_TO_MASK) | from << MOVE_FROM_OFFSET;
}

inline move_t move_set_to(move_t move, square_t to) {
	return (move & MOVE_FROM_MASK) | to;
}

#endif
