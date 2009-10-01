#ifndef CONST_H
#define CONST_H

#define MUSKETEER_COUNT 3
#define MAX_ENEMIES     22

#define BOARD_WIDTH     5
#define BOARD_HEIGHT    5
#define SQUARES         (BOARD_WIDTH * BOARD_HEIGHT)

#define SQUARE_VALID(n) (0 <= (n) && (n) < SQUARES)

#define RULESETS        3

#define MAX_ADJACENTS   4

#define MAX_BRANCHING   44

extern int adjacent[SQUARES][SQUARES];
extern int adjacents[SQUARES][MAX_ADJACENTS];
void init_adjacent(void);
void init_adjacents(void);

extern int dead_pattern_table[RULESETS][SQUARES][SQUARES][SQUARES];
void init_dead_pattern_table(void);

enum player {NOBODY, MUSKETEERS, ENEMIES};
enum square {EMPTY, MUSKETEER, ENEMY};

#endif
