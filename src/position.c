#include <string.h>

#include "const.h"
#include "move.h"
#include "position.h"

#define CHAR_MUSKETEER 'O'
#define CHAR_ENEMY     'X'
#define CHAR_EMPTY     '.'

char square_char[] = {CHAR_MUSKETEER, CHAR_ENEMY, CHAR_EMPTY};

position_t start_position;
position_t error_position;

void init_positions(void) {
	static const enum square start_squares[SQUARES] = {
		ENEMY, ENEMY, ENEMY, ENEMY, MUSKETEER,
		ENEMY, ENEMY, ENEMY, ENEMY, ENEMY,
		ENEMY, ENEMY, MUSKETEER, ENEMY, ENEMY,
		ENEMY, ENEMY, ENEMY, ENEMY, ENEMY,
		MUSKETEER, ENEMY, ENEMY, ENEMY, ENEMY,
	};

	start_position = set_turn(0, MUSKETEERS);
	for (int i = 0; i < SQUARES; ++i)
		start_position = set_square(start_position, i, start_squares[i]);

	error_position = ERROR_BIT;
}

#define PLAYER_CHAR(c) ((c) == 'M' || (c) == 'E')
#define SQUARE_CHAR(c) ((c) == CHAR_MUSKETEER ||\
                        (c) == CHAR_ENEMY ||\
                        (c) == CHAR_EMPTY)

/* [ME] [.OX]{25} */
int read_position(position_t *position, char *string) {
	int i = 0;

	for (; !PLAYER_CHAR(string[i]); ++i)
		if (string[i] == '\0')
			return -1;

	enum player turn = string[i] == 'M' ? MUSKETEERS : ENEMIES;
	*position = set_turn(*position, turn);

	for (int j = 0; j < SQUARES;) {
		for (++i; !SQUARE_CHAR(string[i]); ++i)
			if (string[i] == '\0')
				return -1;
		enum square square = string[i] == CHAR_EMPTY ? EMPTY :
		                     string[i] == CHAR_MUSKETEER ? MUSKETEER :
		                     ENEMY;
		*position = set_square(*position, j++, square);
	}

	return i;
}

/*   a b c d e
 * 1 X X X X O 1
 * 2 X X X X X 2
 * 3 X X O X X 3
 * 4 X X X X X 4
 * 5 O X X X X 5
 *   a b c d e
 * Musketeers to move
 */
int show_position(position_t position, char *string, int N) {
	int len = 12 + 5 * 14 + 12;
	if (N < len + 20)
		return -2;

	strcpy(string,
		"  a b c d e\n"
		"1 ? ? ? ? ? 1\n"
		"2 ? ? ? ? ? 2\n"
		"3 ? ? ? ? ? 3\n"
		"4 ? ? ? ? ? 4\n"
		"5 ? ? ? ? ? 5\n"
		"  a b c d e\n");

	enum player player = winner(position);
	if (player == NOBODY) {
		if (get_turn(position) == MUSKETEERS) {
			strcpy(string + len, "Musketeers to move\n");
			len += 20;
		}
		else if (get_turn(position) == ENEMIES) {
			strcpy(string + len, "Enemies to move\n");
			len += 17;
		}
		else
			return -1;
	}
	else if (player == MUSKETEERS) {
		strcpy(string + len, "Musketeers won\n");
		len += 17;
	}
	else if (player == ENEMIES) {
		strcpy(string + len, "Enemies won\n");
		len += 14;
	}
	else
		return -1;

	for (int y = 0; y < BOARD_HEIGHT; ++y) {
		for (int x = 0; x < BOARD_WIDTH; ++x) {
			int i = y * BOARD_WIDTH + x;
			enum square square = get_square(position, i);
			string[14 + y * 14 + x * 2] = square_char[square];
		}
	}

	return len;
}

inline int dead_pattern(position_t position) {
	uint64_t n = (position & MUSKETEER_MASK) >> MUSKETEER_OFFSET;
	return dead_pattern_table[n];
}

int move_legal(position_t position, move_t move) {
	square_t from = move_get_from(move);
	square_t to = move_get_to(move);

	enum player turn = get_turn(position);

	if (turn == MUSKETEERS)
		return is_musketeer(position, from) &&
		       is_enemy(position, to);
	else
		return is_enemy(position, from) &&
		       is_empty(position, to);
}

int undo_legal(position_t position, move_t undo) {
	square_t from = move_get_from(undo);
	square_t to = move_get_to(undo);
	enum player turn = get_turn(position);

	if (turn == MUSKETEERS) {
		if (!is_empty(position, from) || !is_enemy(position, to))
			return 0;
		if (dead_pattern(position))
			return 0;
	}
	else {
		if (!is_empty(position, from) || !is_musketeer(position, to))
			return 0;
		position = put_musketeer(position, from);
		position = remove_musketeer(position, to);
		if (dead_pattern(position))
			return 0;
	}

	return 1;
}

int move_valid_and_legal(position_t position, move_t move) {
	return move_valid(move) &&
	       !dead_pattern(position) &&
	       move_legal(position, move);
}

int undo_valid_and_legal(position_t position, move_t undo) {
	return move_valid(undo) &&
	       undo_legal(position, undo);
}

position_t apply_move(position_t position, move_t move) {
	square_t from = move_get_from(move);
	square_t to = move_get_to(move);

	if (get_turn(position) == MUSKETEERS) {
		position = remove_musketeer(position, from);
		position = remove_enemy(position, to);
		position = put_musketeer(position, to);
		position = set_turn(position, ENEMIES);
	}
	else {
		position = remove_enemy(position, from);
		position = put_enemy(position, to);
		position = set_turn(position, MUSKETEERS);
	}

	return position;
}

position_t apply_undo(position_t position, move_t undo) {
	square_t from = move_get_from(undo);
	square_t to = move_get_to(undo);

	if (get_turn(position) == MUSKETEERS) {
		position = remove_enemy(position, to);
		position = put_enemy(position, from);
		position = set_turn(position, ENEMIES);
	}
	else {
		position = remove_musketeer(position, to);
		position = put_enemy(position, to);
		position = put_musketeer(position, from);
		position = set_turn(position, MUSKETEERS);
	}

	return position;
}

int list_legal_moves(position_t position, move_t *moves) {
	if (dead_pattern(position))
		return 0;

	int count = 0;

	if (get_turn(position) == MUSKETEERS) {
		for (square_t i = 0; i < SQUARES; ++i) {
			if (!is_musketeer(position, i))
				continue;
			for (square_t j = 0; j < MAX_NEIGHBOURS && neighbours[i][j] >= 0; ++j) {
				if (is_enemy(position, neighbours[i][j])) {
					moves[count] = move_set_from(0, i);
					moves[count] = move_set_to(moves[count], neighbours[i][j]);
					++count;
				}
			}
		}
	}
	else {
		for (square_t i = 0; i < SQUARES; ++i) {
			if (!is_enemy(position, i))
				continue;
			for (square_t j = 0; j < MAX_NEIGHBOURS && neighbours[i][j] >= 0; ++j) {
				if (is_empty(position, neighbours[i][j])) {
					moves[count] = move_set_from(0, i);
					moves[count] = move_set_to(moves[count], neighbours[i][j]);
					++count;
				}
			}
		}
	}

	return count;
}

int list_legal_undos(position_t position, move_t *undos) {
	int count = 0;

	if (get_turn(position) == MUSKETEERS) {
		if (dead_pattern(position))
			return 0;
		for (square_t i = 0; i < SQUARES; ++i) {
			if (!is_empty(position, i))
				continue;
			for (square_t j = 0; j < MAX_NEIGHBOURS && neighbours[i][j] >= 0; ++j) {
				if (is_enemy(position, neighbours[i][j])) {
					undos[count] = move_set_from(0, i);
					undos[count] = move_set_to(undos[count], neighbours[i][j]);
					++count;
				}
			}
		}
	}
	else {
		for (square_t i = 0; i < SQUARES; ++i) {
			if (!is_empty(position, i))
				continue;
			for (square_t j = 0; j < MAX_NEIGHBOURS && neighbours[i][j] >= 0; ++j) {
				if (is_musketeer(position, neighbours[i][j])) {
					undos[count] = move_set_from(0, i);
					undos[count] = move_set_to(undos[count], neighbours[i][j]);
					position_t temp = position;
					temp = put_musketeer(temp, i);
					temp = remove_musketeer(temp, neighbours[i][j]);
					if (!dead_pattern(temp))
						++count;
				}
			}
		}
	}

	return count;
}

int enemies_can_move(position_t position) {
	if (get_turn(position) != ENEMIES || dead_pattern(position))
		return 0;

	for (square_t i = 0; i < SQUARES; ++i) {
		if (!is_enemy(position, i))
			continue;
		for (square_t j = 0; j < MAX_NEIGHBOURS && neighbours[i][j] >= 0; ++j)
			if (is_empty(position, neighbours[i][j]))
				return 1;
	}

	return 0;
}

int musketeers_can_move(position_t position) {
	if (get_turn(position) != MUSKETEERS || dead_pattern(position))
		return 0;

	for (square_t i = 0; i < SQUARES; ++i) {
		if (!is_musketeer(position, i))
			continue;
		for (square_t j = 0; j < MAX_NEIGHBOURS && neighbours[i][j] >= 0; ++j)
			if (is_enemy(position, neighbours[i][j]))
				return 1;
	}

	return 0;
}

enum player winner(position_t position) {
	if (dead_pattern(position))
		return ENEMIES;

	if (get_turn(position) == MUSKETEERS) {
		if (!musketeers_can_move(position))
			return MUSKETEERS;
	}
	else {
		if (!enemies_can_move(position))
			return MUSKETEERS;
	}

	return NOBODY;
}
