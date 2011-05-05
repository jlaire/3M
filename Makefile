all:
	mkdir -p bin data
	gcc -o bin/3M -O2 -std=c99 -D_POSIX_C_SOURCE=200112L -pedantic -Wall -Wextra -Wno-parentheses src/*.c
clean:
	rm bin/*
