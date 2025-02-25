#ifndef UI_H
#define UI_H

#include <ncurses.h>

// Define constants for UI elements
#define DEFAULT     10
#define UNCOVERED   11
#define MINE        12
#define FLAG        13
#define STARTING    14

// Function declarations
void printf_revealed_board(int** board, int size);
void print_board(WINDOW *win, int** board, int size, int cursor_x, int cursor_y, int** revealed, int** flags);
void print_start(WINDOW *win, int size);
void print_lost(WINDOW *win, int** board, int size, int** revealed, int** flags);
void print_win(WINDOW* win);
void print_status(WINDOW *win, int num_mines, int flags_placed, int size, int state, int click);
void init_colors();

#endif // UI_H
