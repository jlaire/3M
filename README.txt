Overview
========

This is a "perfect" AI player for a board game called Three Musketeers. When
the program is in a winning position, it will force a win in as few moves as
possible.

However, when it is in a lost position, it will not resist very hard and even a
human can beat it easily. It would be interesting to experiment with heuristics
for tricking an imperfect opponent into making a mistake and giving away the
win.

Three Musketeers
================

The rules are available at Wikipedia:

	http://en.wikipedia.org/wiki/Three_Musketeers_(game)

Algorithm
=========

The program builds a database that contains the winning side and the number of
moves required for each position. Using the database, the best move for a given
position can be found by looking up trying every possible move and looking up
the resulting position. Then simply pick the one giving the best result.

This approach is feasible because the game has only some millions of
positions.

The database is built using retrogade analysis, starting from the positions
where the game is over and working backwards. One file is used per depth and
and each position takes one byte.

TODO
====

The database generation is very slow. :( Leave it running overnight.
