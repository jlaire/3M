#ifndef RULESET_H
#define RULESET_H

/* The only difference between rulesets is which patterns count as a win for
 * ENEMIES. In the STANDARD ruleset, any horizontal or vertical line wins; in
 * BORDER, lines that lie on one of the 4 borders of the board win; and in
 * HORIZONTAL, only horizontal lines win.
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
enum ruleset {STANDARD, BORDER, HORIZONTAL};

#endif
