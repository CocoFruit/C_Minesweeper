#include <ncurses.h>
#include <string.h>
#include <stdlib.h>

#define DEFAULT     10
#define UNCOVERED   11
#define MINE        12
#define FLAG        13
#define STARTING    14

void printf_revealed_board(int** board, int size) {
    for (int i = 0; i < size; i++) {
        for (int j = 0; j < size; j++) {
            if (board[i][j] == -1) printf(" * ");
            else printf(" %d ", board[i][j]);
        }
        printf("\n");
    }
}
void print_board(WINDOW *win, int** board, int size, int cursor_x, int cursor_y, int** revealed, int** flags){
    wclear(win);
    box(win,0,0);
    char symbol;
    
    for (int y = 0; y < size; y++) {
        for (int x = 0; x < size; x++){
            int index = DEFAULT;
            
            if(revealed[y][x] == 1) {
                symbol = board[y][x] + '0';
                index = board[y][x];
                if(board[y][x] == -1) index = MINE;
                
            } else if(flags[y][x] == 1){
                symbol = 'X';
                index = FLAG;
            } else {
                symbol = ' ';
                index = UNCOVERED;
            }

            if (x == cursor_x && y == cursor_y) {
                wattron(win, A_REVERSE);  // Enable reverse video (reversed colors)
                mvwprintw(win, y + 1, x * 2 + 1, "%c", symbol); // Print the symbol
                wattroff(win, A_REVERSE); // Disable reverse video
            } else {
                wattron(win, COLOR_PAIR(index)); // Apply the color pair normally
                mvwprintw(win, y + 1, x * 2 + 1, "%c", symbol);
                wattroff(win, COLOR_PAIR(index)); // Turn off the color pair
            }
        }
    }
    wrefresh(win);
}
void print_start(WINDOW *win, int *size, int *num_bombs) {
    wclear(win);
    box(win, 0, 0); 
    wattron(win, COLOR_PAIR(STARTING));
    // Remember size is half the window size
    mvwprintw(win, *size / 2 + 1, *size - 8, "[ENTER] TO START");
    wattroff(win, COLOR_PAIR(STARTING));

    mvwprintw(win, *size / 2 + 1 + 2, *size - 8, "#BOMBS");
    mvwprintw(win, *size / 2 + 1 + 3, *size - 6, "/\\");
    mvwprintw(win, *size / 2 + 1 + 4, *size - 6, "%02d", *num_bombs);
    mvwprintw(win, *size / 2 + 1 + 5, *size - 6, "\\/");

    mvwprintw(win, *size / 2 + 1 + 2, *size + 4, "SIZE");
    mvwprintw(win, *size / 2 + 1 + 3, *size + 5, "/\\");
    mvwprintw(win, *size / 2 + 1 + 4, *size + 5, "%02d", *size);
    mvwprintw(win, *size / 2 + 1 + 5, *size + 5, "\\/");


    int selected = 0; // 0 = num_bombs, 1 = size

    if (selected == 0) {
        wattron(win, A_REVERSE);
        mvwprintw(win, *size / 2 + 1 + 4, *size - 6, "%02d", *num_bombs);
        wattroff(win, A_REVERSE);
    } else if (selected == 1) {
        mvwprintw(win, *size / 2 + 1 + 4, *size - 6, "%02d", *num_bombs);
    }

    int ch;
    while ((ch = wgetch(win)) != '\n') {
        switch (ch) {
            case KEY_UP:
                if (selected == 0) {
                    if (*num_bombs < 99) (*num_bombs)++;
                }
                // else {
                //     if (*size < 99) (*size)++;
                // }
                break;
            case KEY_DOWN:
                if (selected == 0) {
                    if (*num_bombs > 1) (*num_bombs)--;
                }
                // else {
                //     if (*size > 1) (*size)--;
                // }
                break;
            case KEY_LEFT:
                selected = 0;
                break;
            case KEY_RIGHT:
                selected = 1;
                break;
            case 'q':
                endwin();
                exit(0);
        }
        wrefresh(win);
        if (selected == 0) {
            wattron(win, A_REVERSE);
            mvwprintw(win, *size / 2 + 1 + 4, *size - 6, "%02d", *num_bombs);
            wattroff(win, A_REVERSE);
            mvwprintw(win, *size / 2 + 1 + 4, *size + 5, "%02d", *size);
        } else if (selected == 1) {
            mvwprintw(win, *size / 2 + 1 + 4, *size - 6, "%02d", *num_bombs);
            wattron(win, A_REVERSE);
            mvwprintw(win, *size / 2 + 1 + 4, *size + 5, "%02d", *size);
            wattroff(win, A_REVERSE);
        }
        wrefresh(win);
    }

    wrefresh(win);
}


void print_lost(WINDOW *win, int** board, int size, int** revealed, int** flags){
    for(int y = 0; y < size; y++){
        for(int x = 0; x < size; x++){
            revealed[y][x] = 1;
        }
    }
    print_board(win, board, size, -1, -1, revealed,flags);
}

void print_win(WINDOW* win) {
    int height, width;
    getmaxyx(win, height, width);
    
    werase(win);
    box(win, 0, 0);
    
    const char* win_message[] = {
        " _    _  _____  _   _  _ ",
        "| |  | ||_   _|| \\ | || |",
        "| |  | |  | |  |  \\| || |",
        "| |/\\| |  | |  | . ` || |",
        "\\  /\\  / _| |_ | |\\  ||_|",
        " \\/  \\/  \\___/ \\_| \\_/(_)",
        "Congratulations! You won!"
    };

    int msg_lines = sizeof(win_message) / sizeof(win_message[0]);
    int start_y = (height - msg_lines) / 2;
    wattron(win, COLOR_PAIR(STARTING));
    for (int i = 0; i < msg_lines; i++) {
        mvwprintw(win, start_y + i, (width - strlen(win_message[i])) / 2, "%s", win_message[i]);
    }
    wattroff(win, COLOR_PAIR(STARTING));
    
    wattron(win, COLOR_PAIR(STARTING));
    mvwprintw(win, start_y + msg_lines + 2, (width - 19) / 2, "Press [r] to restart");
    wattroff(win, COLOR_PAIR(STARTING));
    
    wattron(win, COLOR_PAIR(5));
    mvwprintw(win, start_y + msg_lines + 2, (width - 22) / 2, "Press [v] to view board");
    wattroff(win, COLOR_PAIR(5));

    wrefresh(win);
    
    nodelay(win, FALSE); // Wait for user input
    wgetch(win);
}


void print_status(WINDOW *win, int num_mines, int flags_placed, int size, int state, int click){
    mvwprintw(win, 1, 2, "Mines: %02d", num_mines);


    
    if (click == 0){ // normal face
        if (state == -1){ // dead face
            wattron(win, COLOR_PAIR(FLAG));
            mvwprintw(win, 1, size*2-3-1, "X_X");
            wattroff(win, COLOR_PAIR(FLAG));
        }
        else{
            wattron(win, COLOR_PAIR(STARTING));
            mvwprintw(win, 1, size*2-3-1, "^_^");
            wattroff(win, COLOR_PAIR(STARTING));
        }
    }
    else
    if (click == 1){ // surprised face
        wattron(win, COLOR_PAIR(FLAG));
        mvwprintw(win, 1, size*2-3-1, "O_O");
        wattroff(win, COLOR_PAIR(FLAG));
    }


    int flag_color = (int) (((float) flags_placed / (float) num_mines) * (9-1) + 1);
    wattron(win, COLOR_PAIR(flag_color));
    mvwprintw(win, 2, 2, "Flags: %02d", flags_placed);
    wattroff(win, COLOR_PAIR(flag_color));

    if(state == -1){
        wattron(win, COLOR_PAIR(FLAG));
        mvwprintw(win, 3, 2, "You lost!");
        wattroff(win, COLOR_PAIR(FLAG));
    }

    else if(state == 1){
        wattron(win, COLOR_PAIR(FLAG));
        mvwprintw(win, 3, 2, "You won!");
        wattroff(win, COLOR_PAIR(FLAG));
    }

    else {
        mvwprintw(win, 3, 2, "          ");
    }

    wrefresh(win);
}



void init_colors(){
    init_pair(DEFAULT, COLOR_BLUE, COLOR_BLACK);
    init_pair(UNCOVERED, COLOR_WHITE, COLOR_BLACK);
    init_pair(MINE, COLOR_RED, COLOR_RED);
    init_pair(FLAG, COLOR_RED, COLOR_BLACK);
    init_pair(STARTING, COLOR_GREEN, COLOR_BLACK);

    // thank you ChatGPT
    // Cold colors (blue-ish) for 1-3
    init_pair(1, COLOR_BLUE, COLOR_BLACK);      // 1 - Cold
    init_pair(2, COLOR_CYAN, COLOR_BLACK);      // 2 - Slightly warmer
    init_pair(3, COLOR_GREEN, COLOR_BLACK);     // 3 - Even warmer

    // Moderate colors for 4-6
    init_pair(4, COLOR_YELLOW, COLOR_BLACK);    // 4 - Getting warmer
    init_pair(5, COLOR_MAGENTA, COLOR_BLACK);   // 5 - Quite warm
    init_pair(6, COLOR_RED, COLOR_BLACK);       // 6 - Hot

    // Hot colors for 7-9
    init_pair(7, COLOR_WHITE, COLOR_BLACK);     // 7 - Very hot
    init_pair(8, COLOR_BLACK, COLOR_WHITE);     // 8 - Extremely hot (inverse effect)
    init_pair(9, COLOR_YELLOW, COLOR_WHITE);    // 9 - Blazing hot (inverse effect)

}