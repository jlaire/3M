#include <string.h>

#include "const.h"
#include "ruleset.h"
#include "move.h"
#include "position.h"

#define CHAR_EMPTY     '.'
#define CHAR_MUSKETEER 'O'
#define CHAR_ENEMY     'X'

char square_char[] = { CHAR_EMPTY, CHAR_MUSKETEER, CHAR_ENEMY };

/* The start position is currently identical in all rulesets */
void set_to_start_position(struct position *position) {
	int i;

	static enum square start_squares[SQUARES] = {
		ENEMY, ENEMY, ENEMY, ENEMY, MUSKETEER,
		ENEMY, ENEMY, ENEMY, ENEMY, ENEMY,
		ENEMY, ENEMY, MUSKETEER, ENEMY, ENEMY,
		ENEMY, ENEMY, ENEMY, ENEMY, ENEMY,
		MUSKETEER, ENEMY, ENEMY, ENEMY, ENEMY,
	};

	position->player_to_move = MUSKETEERS;
	for (i = 0; i < SQUARES; ++i)
		position->squares[i] = start_squares[i];
}

int count_musketeers(struct position *position) {
	int i;
	int count;

	for (count = i = 0; i < SQUARES; ++i)
		if (position->squares[i] == MUSKETEER)
			++count;

	return count;
}

int count_enemies(struct position *position) {
	int i;
	int count;

	for (count = i = 0; i < SQUARES; ++i)
		if (position->squares[i] == ENEMY)
			++count;

	return count;
}

#define RULESET_CHAR(c) ((c) == 'S' || (c) == 'B' || (c) == 'H')
#define PLAYER_CHAR(c) ((c) == 'M' || (c) == 'E')
#define SQUARE_CHAR(c) ((c) == CHAR_EMPTY ||\
			(c) == CHAR_MUSKETEER ||\
			(c) == CHAR_ENEMY)

/* [SBH] [ME] [.OX]{25} */
int read_position(struct position *position, char *string) {
	int i = 0;
	int square;

	for (; !RULESET_CHAR(string[i]); ++i)
		if (string[i] == '\0')
			return -1;

	position->ruleset =
		string[i] == 'B' ? BORDER :
		string[i] == 'H' ? HORIZONTAL :
		STANDARD;

	for (++i; !PLAYER_CHAR(string[i]); ++i)
		if (string[i] == '\0')
			return -1;

	position->player_to_move = string[i] == 'M' ? MUSKETEERS : ENEMIES;

	for (square = 0; square < SQUARES; ++square) {
		for (++i; !SQUARE_CHAR(string[i]); ++i)
			if (string[i] == '\0')
				return -1;
		position->squares[square] =
			string[i] == CHAR_EMPTY ? EMPTY :
			string[i] == CHAR_MUSKETEER ? MUSKETEER :
			ENEMY;
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
int show_position(struct position *position, char *string, int N) {
	int y;
	int x;
	int len;
	enum player player;

	len = 12 + 5 * 14 + 12;
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

	player = winner(position);
	if (player == NOBODY) {
		if (position->player_to_move == MUSKETEERS) {
			strcpy(string + len, "Musketeers to move\n");
			len += 20;
		}
		else if (position->player_to_move == ENEMIES) {
			strcpy(string + len, "Enemies to move\n");
			len += 17;
		}
		else
			return -1;
	}
	else if (player == MUSKETEERS) {
		strcpy(string + len, "Musketeers win!\n");
		len += 17;
	}
	else if (player == ENEMIES) {
		strcpy(string + len, "Enemies win!\n");
		len += 14;
	}
	else
		return -1;

	for (y = 0; y < BOARD_HEIGHT; ++y) {
		for (x = 0; x < BOARD_WIDTH; ++x) {
			enum square square =
				position->squares[y * BOARD_WIDTH + x];
			string[14 + y * 14 + x * 2] = square_char[square];
		}
	}

	return len;
}

int dead_pattern(struct position *position) {
	int is[SQUARES];
	int musketeers = 0;
	int i;

	for (i = 0; i < SQUARES; ++i)
		if (position->squares[i] == MUSKETEER)
			is[musketeers++] = i;

	return musketeers == 3 &&
	       dead_pattern_table[position->ruleset][is[0]][is[1]][is[2]];
}

int move_legal(struct position *position, struct move *move) {
	enum square from;
	enum square to;

	if (!move_valid(move))
		return 0;

	if (dead_pattern(position))
		return 0;

	from = position->squares[move->from];
	to = position->squares[move->to];

	if (position->player_to_move == MUSKETEERS)
		return from == MUSKETEER && to == ENEMY;
	else if (position->player_to_move == ENEMY)
		return from == ENEMY && to == EMPTY;
	else
		return 0;
}

int undo_legal(struct position *position, struct move *undo) {
	enum square from;
	enum square to;
	int temp;

	if (!move_valid(undo))
		return 0;

	from = position->squares[undo->from];
	to = position->squares[undo->to];

	if (position->player_to_move == MUSKETEERS)
		if (from != EMPTY || to != ENEMY)
			return 0;
	else if (position->player_to_move == ENEMY)
		if (from != EMPTY && to != MUSKETEER)
			return 0;
	else
		return 0;

	/* TODO: Is this good enough? */
	apply_undo(position, undo);
	temp = dead_pattern(position);
	apply_move(position, undo);

	return !temp;
}

int apply_move(struct position *position, struct move *move) {
	switch (position->player_to_move) {
	case MUSKETEERS:
		position->squares[move->from] = EMPTY;
		position->squares[move->to] = MUSKETEER;
		position->player_to_move = ENEMIES;
		return 0;
	case ENEMIES:
		position->squares[move->from] = EMPTY;
		position->squares[move->to] = ENEMY;
		position->player_to_move = MUSKETEERS;
		return 0;
	default:
		return 1;
	}
}

int apply_undo(struct position *position, struct move *undo) {
	switch (position->player_to_move) {
	case MUSKETEERS:
		position->squares[undo->from] = ENEMY;
		position->squares[undo->to] = EMPTY;
		position->player_to_move = ENEMIES;
		return 0;
	case ENEMIES:
		position->squares[undo->from] = MUSKETEER;
		position->squares[undo->to] = ENEMY;
		position->player_to_move = MUSKETEERS;
		return 0;
	default:
		return 1;
	}
}

int list_legal_moves(struct position *position, struct move *moves) {
	int count = 0;
	int i;
	int j;

	if (dead_pattern(position))
		return 0;

	if (position->player_to_move == MUSKETEERS) {
		for (i = 0; i < SQUARES; ++i) {
			if (position->squares[i] != MUSKETEER)
				continue;
			for (j = 0; j < MAX_ADJACENTS && adjacents[i][j] >= 0; ++j) {
				if (position->squares[adjacents[i][j]] == ENEMY) {
					moves[count].from = i;
					moves[count].to = adjacents[i][j];
					++count;
				}
			}
		}
	}

	if (position->player_to_move == ENEMIES) {
		for (i = 0; i < SQUARES; ++i) {
			if (position->squares[i] != ENEMY)
				continue;
			for (j = 0; j < MAX_ADJACENTS && adjacents[i][j] >= 0; ++j) {
				if (position->squares[adjacents[i][j]] == EMPTY) {
					moves[count].from = i;
					moves[count].to = adjacents[i][j];
					++count;
				}
			}
		}
	}

	return count;
}

int list_legal_undos(struct position *position, struct move *undos) {
	int count = 0;
	int i;
	int j;

	if (position->player_to_move == MUSKETEERS) {
		if (dead_pattern(position))
			return 0;
		for (i = 0; i < SQUARES; ++i) {
			if (position->squares[i] != EMPTY)
				continue;
			for (j = 0; j < MAX_ADJACENTS && adjacents[i][j] >= 0; ++j) {
				if (position->squares[adjacents[i][j]] == ENEMY) {
					undos[count].from = i;
					undos[count].to = adjacents[i][j];
					++count;
				}
			}
		}
	}

	if (position->player_to_move == ENEMIES) {
		for (i = 0; i < SQUARES; ++i) {
			if (position->squares[i] != EMPTY)
				continue;
			for (j = 0; j < MAX_ADJACENTS && adjacents[i][j] >= 0; ++j) {
				if (position->squares[adjacents[i][j]] == MUSKETEER) {
					undos[count].from = i;
					undos[count].to = adjacents[i][j];
					/* TODO: This is kinda lame */
					if (undo_legal(position, &undos[count]))
						++count;
				}
			}
		}
	}

	return count;
}

int enemies_can_move(struct position *position) {
	int i;
	int j;

	if (position->player_to_move != ENEMIES ||
	    dead_pattern(position))
	{
		return 0;
	}

	for (i = 0; i < SQUARES; ++i) {
		if (position->squares[i] != ENEMY)
			continue;
		for (j = 0; j < MAX_ADJACENTS && adjacents[i][j] >= 0; ++j) {
			if (position->squares[adjacents[i][j]] == EMPTY) {
				return 1;
			}
		}
	}

	return 0;
}

int musketeers_can_move(struct position *position) {
	int i;
	int j;

	if (position->player_to_move != MUSKETEERS ||
	    dead_pattern(position))
	{
		return 0;
	}

	for (i = 0; i < SQUARES; ++i) {
		if (position->squares[i] != MUSKETEER)
			continue;
		for (j = 0; j < MAX_ADJACENTS && adjacents[i][j] >= 0; ++j) {
			if (position->squares[adjacents[i][j]] == ENEMY) {
				return 1;
			}
		}
	}

	return 0;
}

enum player winner(struct position *position) {
	if (dead_pattern(position))
		return ENEMIES;

	if (position->player_to_move == MUSKETEERS &&
	    !musketeers_can_move(position))
	{
		return MUSKETEERS;
	}

	if (position->player_to_move == ENEMIES &&
	    !enemies_can_move(position))
	{
		return MUSKETEERS;
	}

	return NOBODY;
}
