#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <ncurses.h>
#include <unistd.h>
#include <string.h>
#include "ui.h"
#include "game_logic.h"

int main() {
    srand(time(NULL));

    const int NUM_MINES = 30;
    const int BOARD_SIZE = 17;
    int** revealed;
    int** flags;
    int** board = create_board(BOARD_SIZE, NUM_MINES, &revealed, &flags);
    
    if (!board) {
        printf("Memory allocation failed.\n");
        return 1;
    }

    initscr();
    noecho();
    curs_set(0);
    int rows, cols;
    getmaxyx(stdscr, rows, cols);

    int start_y = (rows - (BOARD_SIZE + 2)) / 2;
    int start_x = (cols - (BOARD_SIZE * 2 + 1)) / 2;

    WINDOW *board_win = newwin(BOARD_SIZE + 2, BOARD_SIZE * 2 + 1, start_y, start_x);
    keypad(board_win, TRUE);

    int status_height = 5;
    WINDOW *status_win = newwin(status_height, BOARD_SIZE * 2 + 1, start_y + BOARD_SIZE + 2, start_x);
    box(status_win, 0, 0);

    start_color();
    init_colors();

    int ch;
    int curx = BOARD_SIZE / 2, cury = BOARD_SIZE / 2;
    int flags_placed = 0;
    int state = 0;

    print_start(board_win, BOARD_SIZE);
    print_board(board_win, board, BOARD_SIZE, curx, cury, revealed, flags);
    print_status(status_win, NUM_MINES, flags_placed, BOARD_SIZE, state, 0);

    while ((ch = wgetch(board_win)) != 'q') {
        switch (ch) {
            case KEY_UP: 
                if (cury > 0) cury--;
                break;
            case KEY_DOWN: 
                if (cury < BOARD_SIZE - 1) cury++;
                break;
            case KEY_LEFT: 
                if (curx > 0) curx--;
                break;
            case KEY_RIGHT: 
                if (curx < BOARD_SIZE - 1) curx++;
                break;
            case ' ': 
                if (revealed[cury][curx] == 1) break;
                print_status(status_win, NUM_MINES, flags_placed, BOARD_SIZE, state, 1);
                napms(100);
                if (board[cury][curx] == -1){
                    state = -1;
                    print_lost(board_win, board, BOARD_SIZE, revealed, flags);
                }
                if (board[cury][curx] == 0){
                    flood_reveal(board, curx, cury, BOARD_SIZE, revealed, flags);
                } 
                revealed[cury][curx] = 1;
                if (check_win(board, BOARD_SIZE, revealed) && state == 0) {
                    print_win(board_win);
                    state = 1;
                }
                break;
            case 'f': 
            case 'x': 
                if (revealed[cury][curx] == 1) break;
                flags[cury][curx] = (flags[cury][curx] + 1) % 2;
                flags_placed += flags[cury][curx] == 1 ? 1 : -1;
                break;
            case 'r':
                board = create_board(BOARD_SIZE, NUM_MINES, &revealed, &flags);
                print_start(board_win, BOARD_SIZE);
                state = 0;
                break;
            default:
                break;
        }

        print_board(board_win, board, BOARD_SIZE, curx, cury, revealed, flags);
        print_status(status_win, NUM_MINES, flags_placed, BOARD_SIZE, state, 0);
    }

    endwin();

    for (int i = 0; i < BOARD_SIZE; i++) {
        free(board[i]);
        free(revealed[i]);
        free(flags[i]);
    }
    free(board);
    free(revealed);
    free(flags);

    return 0;
}
