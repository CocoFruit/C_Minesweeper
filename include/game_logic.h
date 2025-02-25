#ifndef GAME_LOGIC_H
#define GAME_LOGIC_H

#include <stdbool.h>  // For the bool type

void placeMines(int** board, int size, int num_mines);
void placeNums(int** board, int size);
int** create_board(int size, int num_mines, int*** revealed, int*** flags);
void flood_reveal(int** board, int x, int y, int size, int** revealed, int** flags);
bool check_win(int** board, int size, int** revealed);

#endif
