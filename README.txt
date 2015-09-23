Overview
========

This is a "perfect" AI player for a board game called Three Musketeers. When
the program is in a winning position, it will force a win in as few moves as
possible.

However, when it is in a lost position, it will not resist very hard and even a
human can beat it easily. It would be interesting to experiment with heuristics
for tricking an imperfect opponent into making a mistake and giving away the
win.

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

Usage
=====

Build with `make`.

First the database need to be generated. This will require 2.5GB of space.

```
$ time ./build/3M -g
Generating file data/00M... done
Generating file data/00E... done
Generating file data/01M... done
Generating file data/01E... done
[...]
Generating file data/21M... done
Generating file data/21E... done
Generating file data/22M... done
Generating file data/22E... done

real    476m51.631s
user    476m4.812s
sys     0m4.601s
```

Then you can play interactively with `-i` or query the database with `-s`.

```
$ ./build/3M -s <<< 'M XXXXO XXXXX XXOXX XXXXX OXXXX'
E33 -> [e1-d1 E32, e1-e2 E32, c3-c2 E32, c3-b3 E32, c3-d3 E32, c3-c4 E32, a5-a4 E32, a5-b5 E32]

$ ./build/3M -s <<< 'E XXXO. XXXXX XXOXX XXXXX OXXXX'
E32 -> [e2-e1 E31]
```

The first character in the input indicates which side is to play.

The output starts with the winning side and the number of moves required to
win. Then all legal moves and their optimal results are listed.

TODO
====

- The database generation is not optimized and takes a looooong time.
- JSON instead of ad-hoc plain text.
- Accept path to the data folder as a command-line argument.
