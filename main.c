#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <ncurses.h>
#include <unistd.h>
#include <string.h>
#include "ui.h"
#include "game_logic.h"


void free_memory(int** board, int** revealed, int** flags, int size) {
    for (int i = 0; i < size; i++) {
        free(board[i]);
        free(revealed[i]);
        free(flags[i]);
    }
    free(board);
    free(revealed);
    free(flags);
}

int main() {
    srand(time(NULL));

    int num_mines = 30;
    int board_size = 17;
    int** revealed;
    int** flags;


    initscr();
    noecho();
    curs_set(0);
    int rows, cols;
    getmaxyx(stdscr, rows, cols);

    int start_y = (rows - (board_size + 2)) / 2;
    int start_x = (cols - (board_size * 2 + 1)) / 2;

    WINDOW *board_win = newwin(board_size + 2, board_size * 2 + 1, start_y, start_x);
    keypad(board_win, TRUE);

    int status_height = 5;
    WINDOW *status_win = newwin(status_height, board_size * 2 + 1, start_y + board_size + 2, start_x);
    box(status_win, 0, 0);

    start_color();
    init_colors();

    // Display start screen
    print_start(board_win, &board_size, &num_mines);


    int ch;
    int curx = board_size / 2, cury = board_size / 2;
    int flags_placed = 0;
    int state = 0; // 0 = playing, -1 = lost, 1 = won
    int moved = 0;

    int** board = create_board(board_size, num_mines, &revealed, &flags);
    
    if (!board) {
        printf("Memory allocation failed.\n");
        return 1;
    }

    print_board(board_win, board, board_size, curx, cury, revealed, flags);
    print_status(status_win, num_mines, flags_placed, board_size, state, 0);

    while ((ch = wgetch(board_win)) != 'q') {
        switch (ch) {
            case KEY_UP: 
                if (cury > 0) cury--;
                break;
            case KEY_DOWN: 
                if (cury < board_size - 1) cury++;
                break;
            case KEY_LEFT: 
                if (curx > 0) curx--;
                break;
            case KEY_RIGHT: 
                if (curx < board_size - 1) curx++;
                break;
            case ' ': 
                if (revealed[cury][curx] == 1) break;
                print_status(status_win, num_mines, flags_placed, board_size, state, 1);
                napms(100);
                
                if(moved == 0){
                    // make sure the first click is not a mine
                    /* THIS COULD BE OPTIMIZED */
                    while(board[cury][curx] != 0){
                        free_memory(board, revealed, flags, board_size);
                        board = create_board(board_size, num_mines, &revealed, &flags);
                    }
                    moved = 1;
                }
                
                if (board[cury][curx] == -1){
                    state = -1;
                    print_lost(board_win, board, board_size, revealed, flags);
                }
                if (board[cury][curx] == 0){
                    flood_reveal(board, curx, cury, board_size, revealed, flags);
                } 
                revealed[cury][curx] = 1;
                if (check_win(board, board_size, revealed) && state == 0) {
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
                free_memory(board, revealed, flags, board_size);
                print_start(board_win, &board_size, &num_mines);

                board = create_board(board_size, num_mines, &revealed, &flags);
                print_board(board_win, board, board_size, curx, cury, revealed, flags);
                state = 0;
                flags_placed = 0;
                moved = 0;
                break;
            default:
                break;
        }

        print_board(board_win, board, board_size, curx, cury, revealed, flags);
        print_status(status_win, num_mines, flags_placed, board_size, state, 0);
    }

    endwin();

    free_memory(board, revealed, flags, board_size);

    return 0;
}
