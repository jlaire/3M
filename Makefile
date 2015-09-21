CC = gcc
CFLAGS = -std=c99 -O2 -D_POSIX_C_SOURCE=200112L -pedantic -Wall -Wextra

all:
	mkdir -p build data
	$(CC) $(CFLAGS) -o build/3M src/*.c

clean:
	rm -rf build
