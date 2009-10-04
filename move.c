#include "const.h"
#include "move.h"

int move_valid(move_t move) {
	square_t from = move_get_from(move);
	square_t to = move_get_to(move);
	return	SQUARE_VALID(from) &&
		SQUARE_VALID(to) &&
		adjacent[from][to];
}

#define FILE_CHAR(c) ('a' <= (c) && (c) < 'a' + BOARD_WIDTH)
#define RANK_CHAR(c) ('1' <= (c) && (c) < '1' + BOARD_HEIGHT)

/* [a-e][1-5] */
int read_square(square_t *square, char *string) {
	unsigned int len = 0;

	for (; !FILE_CHAR(string[len]); ++len)
		if (string[len] == '\0')
			return -1;

	*square = string[len++] - 'a';

	for (; !RANK_CHAR(string[len]); ++len)
		if (string[len] == '\0')
			return -1;

	*square += (string[len++] - '1') * BOARD_WIDTH;
	return len;
}

/* <from-square> <to-square> */
int read_move(move_t *move, char *string) {
	square_t from;
	int len_from = read_square(&from, string);
	if (len_from < 0)
		return len_from;

	square_t to;
	int len_to = read_square(&to, string + len_from);
	if (len_to < 0)
		return len_to;

	*move = move_set_from(*move, from);
	*move = move_set_to(*move, to);

	if (!move_valid(*move))
		return -1;

	return len_from + len_to;
}

int show_square(square_t square, char *string, int N) {
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
	if (!move_valid(move))
		return -1;

	int len_from = show_square(move_get_from(move), string, N);
	if (len_from < 0)
		return len_from;

	string[len_from] = '-';

	int len_to = show_square(move_get_to(move),
	                         string + len_from + 1,
	                         N - len_from - 1);
	if (len_to < 0)
		return len_to;

	string[len_to + 1 + len_from] = '\0';

	return len_from + 1 + len_to;
}
