#ifndef CONST_H
#define CONST_H

#include <stdint.h>

#define MUSKETEER_COUNT 3
#define MAX_ENEMIES     22

#define BOARD_WIDTH     5
#define BOARD_HEIGHT    5
#define SQUARES         (BOARD_WIDTH * BOARD_HEIGHT)

#define SQUARE_VALID(n) (0 <= (n) && (n) < SQUARES)

#define RULESETS        3

#define MAX_ADJACENTS   4

/* O . X . O
 * . X . X .
 * X . X . X
 * . X . X .
 * O . X . X
 */
#define MAX_BRANCHING   34

extern int adjacent[SQUARES][SQUARES];
extern int adjacents[SQUARES][MAX_ADJACENTS];
void init_adjacent(void);
void init_adjacents(void);

extern uint8_t count_bits_25[1 << 25];
void init_count_bits_25(void);

extern uint8_t dead_pattern_table[RULESETS][1 << 25];
void init_dead_pattern_table(void);

enum player {MUSKETEERS, ENEMIES, NOBODY};
enum square {MUSKETEER, ENEMY, EMPTY};

#endif
