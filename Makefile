all:
	mkdir -p bin data
	gcc -o bin/3M -O2 -std=c99 -pedantic -Wall -Wextra -Wno-parentheses src/*.c
clean:
	rm bin/*
