#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <ncurses.h>

void placeMines(int** board, int size, int num_mines) {
    int total_cells = size * size;
    int* positions = malloc(total_cells * sizeof(int));

    for (int i = 0; i < total_cells; i++) positions[i] = i;

    for (int i = 0; i < num_mines; i++) {
        int j = i + rand() % (total_cells - i);
        int temp = positions[i];
        positions[i] = positions[j];
        positions[j] = temp;
    }

    for (int i = 0; i < num_mines; i++) {
        int row = positions[i] / size;
        int col = positions[i] % size;
        board[row][col] = -1;
    }

    free(positions);
}

void placeNums(int** board, int size) {
    for (int y = 0; y < size; y++) {
        for (int x = 0; x < size; x++) {
            if (board[y][x] == -1) {
                for (int ty = -1; ty <= 1; ty++) {
                    for (int tx = -1; tx <= 1; tx++) {
                        int ny = y + ty;
                        int nx = x + tx;
                        if (ny >= 0 && ny < size && nx >= 0 && nx < size) {
                            if (board[ny][nx] != -1) board[ny][nx]++;
                        }
                    }
                }
            }
        }
    }
}

int** create_board(int size, int num_mines, int*** revealed, int*** flags) {
    int** board = malloc(size * sizeof(int*));
    *revealed = malloc(size * sizeof(int*));
    *flags = malloc(size * sizeof(int*));

    if (!board) return NULL;

    for (int i = 0; i < size; i++) {
        board[i] = calloc(size, sizeof(int));
        (*revealed)[i] = calloc(size, sizeof(int));
        (*flags)[i] = calloc(size, sizeof(int));
        
        if (!board[i]) {
            for (int j = 0; j < i; j++) free(board[j]);
            free(board);
            return NULL;
        }
    }

    placeMines(board, size, num_mines);
    placeNums(board, size);

    return board;
}

void flood_reveal(int** board, int x, int y, int size, int** revealed, int** flags) {
    if (x < 0 || x >= size || y < 0 || y >= size || revealed[y][x] == 1 || flags[y][x] == 1) {
        return;
    }

    revealed[y][x] = 1;

    if (board[y][x] == 0) {
        for (int tx = -1; tx <= 1; tx++) {
            for (int ty = -1; ty <= 1; ty++) {
                if (tx == 0 && ty == 0) continue;
                flood_reveal(board, x + tx, y + ty, size, revealed, flags);
            }
        }
    }
}

bool check_win(int** board, int size, int** revealed) {
    for (int y = 0; y < size; y++) {
        for (int x = 0; x < size; x++) {
            if (board[y][x] != -1 && revealed[y][x] == 0) {
                return false;
            }
        }
    }
    return true;
}
