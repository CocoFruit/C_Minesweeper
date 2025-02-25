CC = gcc
CFLAGS = -Wall -Wextra -pedantic -std=c99
BIN_DIR = bin

# Targets
mine: minesweeper.c
	$(CC) $(CFLAGS) minesweeper.c -o $(BIN_DIR)/mine -lncurses

clean:
	rm -f $(BIN_DIR)/mine
