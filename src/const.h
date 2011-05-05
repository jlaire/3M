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

/* Counts the number of set bits in n */
unsigned int popcnt(unsigned int n);

int is_dead_pattern(uint32_t);

enum player {MUSKETEERS, ENEMIES, NOBODY};
enum square {MUSKETEER, ENEMY, EMPTY};

#endif
