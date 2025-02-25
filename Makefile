CC = gcc
CFLAGS = -Wall -Wextra -pedantic -std=c99

mine: minesweeper.c
	$(CC) $(CFLAGS) minesweeper.c -o mine -lncurses

clean:
	rm -f mine
