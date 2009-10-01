#ifndef RULESET_H
#define RULESET_H

/* Currently, the only difference between rulesets is what
 * counts as a win for ENEMIES. In STANDARD, any horizontal
 * or vertical line; in BORDER, any line along any of the 4
 * borders of the board; and in HORIZONTAL, only horizontal
 * lines.
 *
 * +-+-+-+-+   +-------+   ---------
 * | | | | |   |       |
 * +-+-+-+-+   |       |   ---------
 * | | | | |   |       |
 * +-+-+-+-+   |       |   ---------
 * | | | | |   |       |
 * +-+-+-+-+   |       |   ---------
 * | | | | |   |       |
 * +-+-+-+-+   +-------+   ---------
 * STANDARD     BORDER     HORIZONTAL
 */
enum ruleset { STANDARD, BORDER, HORIZONTAL };

#endif
