#include "const.h"
#include "move.h"

int move_get_from(move_t move) {
	return move >> 8;
}

int move_get_to(move_t move) {
	return move & MOVE_TO_MASK;
}

move_t move_set_from(move_t move, int from) {
	return move & MOVE_TO_MASK | from << 8;
}

move_t move_set_to(move_t move, int to) {
	return move & MOVE_FROM_MASK | to;
}

int move_valid(move_t move) {
	int from = move_get_from(move);
	int to = move_get_to(move);
	return	SQUARE_VALID(from) &&
		SQUARE_VALID(to) &&
		adjacent[from][to];
}

#define FILE_CHAR(c) ('a' <= (c) && (c) < 'a' + BOARD_WIDTH)
#define RANK_CHAR(c) ('1' <= (c) && (c) < '1' + BOARD_HEIGHT)

/* [a-e][1-5] */
int read_square(int *square, char *string) {
	int i = 0;

	for (; !FILE_CHAR(string[i]); ++i)
		if (string[i] == '\0')
			return -1;

	*square = string[i++] - 'a';

	for (; !RANK_CHAR(string[i]); ++i)
		if (string[i] == '\0')
			return -1;

	*square += (string[i++] - '1') * BOARD_WIDTH;
	return i;
}

/* <from-square> <to-square> */
int read_move(move_t *move, char *string) {
	int len_from;
	int len_to;
	int from;
	int to;

	len_from = read_square(&from, string);
	if (len_from < 0)
		return len_from;

	len_to = read_square(&to, string + len_from);
	if (len_to < 0)
		return len_to;

	*move = move_set_from(*move, from);
	*move = move_set_to(*move, to);

	if (!move_valid(*move))
		return -1;

	return len_from + len_to;
}

int show_square(int square, char *string, int N) {
	if (!SQUARE_VALID(square))
		return -1;
	if (N < 3)
		return -2;

	*string++ = (square % BOARD_WIDTH) + 'a';
	*string++ = (square / BOARD_WIDTH) + '1';
	*string = '\0';

	return 2;
}

int show_move(move_t move, char *string, int N) {
	int len_from;
	int len_to;

	if (!move_valid(move))
		return -1;

	len_from = show_square(move_get_from(move), string, N);
	if (len_from < 0)
		return len_from;

	string[len_from] = '-';

	len_to = show_square(move_get_to(move), string + len_from + 1, N - len_from - 1);
	if (len_to < 0)
		return len_to;

	string[len_to + 1 + len_from] = '\0';

	return len_from + 1 + len_to;
}
