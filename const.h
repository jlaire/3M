#ifndef CONST_H
#define CONST_H

#include <stdint.h>

#define BOARD_WIDTH     5
#define BOARD_HEIGHT    5

typedef unsigned int square_t;

#define SQUARES         (BOARD_WIDTH * BOARD_HEIGHT)

#define SQUARE_VALID(n) ((n) < SQUARES)

#define MUSKETEER_COUNT 3
#define MAX_ENEMIES     (SQUARES - MUSKETEER_COUNT)

#define RULESETS        3

#define MAX_NEIGHBOURS  4

/* O . X . O
 * . X . X .
 * X . X . X
 * . X . X .
 * O . X . X
 */
#define MAX_BRANCHING   34

extern int adjacent[SQUARES][SQUARES];
extern int neighbours[SQUARES][MAX_NEIGHBOURS];
void init_adjacent(void);
void init_neighbours(void);

unsigned int count_bits_25(unsigned int n);

extern uint8_t dead_pattern_table[RULESETS][1 << 25];
void init_dead_pattern_table(void);

enum player {MUSKETEERS, ENEMIES, NOBODY};
enum square {MUSKETEER, ENEMY, EMPTY};

#endif
