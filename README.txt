Overview
========

This small C project implements a "perfect" AI player for a board game called
Three Musketeers. When the AI is in a winning position, it will win in as few
moves as possible. However, when it is in a lost position and playing against a
non-perfect opponent, it doesn't fight very hard and has room for improvement.

Three Musketeers
================

This board game has very simple rules. You can find them and more information
with Google or at the Wikipedia article:

	http://en.wikipedia.org/wiki/Three_Musketeers_(game)

Algorithm
=========

The program builds a database that contains the winner and number of moves
required for each position. The best next move can be found by looking up the
position following each possible move and selecting the one with best result.

This approach is feasible because the game has only some millions of
positions.

The database is built using retrogade analysis, starting from the positions
where the game is over and working backwards. One file is used per depth and
and each position takes one byte.

TODO
====

The database generation is very slow and overall the code could be more robust.
