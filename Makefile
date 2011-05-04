all:
	mkdir -p DB/{STANDARD,BORDER,HORIZONTAL}
	gcc -o 3M -O2 -std=c99 -pedantic -Wall -Wextra -Wno-parentheses *.c
clean:
	rm 3M *.o
