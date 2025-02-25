#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <ncurses.h>
#include <unistd.h> // For sleep
#include <string.h>

#define DEFAULT     10
#define UNCOVERED   11
#define MINE        12
#define FLAG        13
#define STARTING    14

void placeMines(int** board, int size, int num_mines) {
    int total_cells = size * size;
    int* positions = malloc(total_cells * sizeof(int)); // Store indices

    // Fill positions array
    for (int i = 0; i < total_cells; i++) positions[i] = i;

    // Shuffle first `num_mines` positions
    for (int i = 0; i < num_mines; i++) {
        int j = i + rand() % (total_cells - i);
        int temp = positions[i];
        positions[i] = positions[j];
        positions[j] = temp;
    }

    // Place mines at shuffled positions
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
    int** board = malloc(size * sizeof(int*)); // Allocate row pointers
    *revealed = malloc(size * sizeof(int*));
    *flags = malloc(size * sizeof(int*));

    if (!board) return NULL; // Memory allocation failed

    for (int i = 0; i < size; i++) {
        board[i] = calloc(size, sizeof(int)); // Allocate and zero-initialize
        (*revealed)[i] = calloc(size, sizeof(int)); // Init all to false (0)
        (*flags)[i] = calloc(size, sizeof(int)); // Init all to false (0)
        
        if (!board[i]) {
            // Free already allocated memory before returning NULL
            for (int j = 0; j < i; j++) free(board[j]);
            free(board);
            return NULL;
        }
    }

    placeMines(board, size, num_mines);
    placeNums(board, size);

    return board;
}

void flood_reveal(WINDOW *win, int** board, int x, int y, int size, int** revealed, int**flags) {
    if (x < 0 || x >= size || y < 0 || y >= size || revealed[y][x] == 1 || flags[y][x] == 1) {
        return;  // Exit if out of bounds or already revealed or if its a flag
    }

    revealed[y][x] = 1;  // Reveal the current tile

    // If it's a zero, recursively reveal neighbors
    if (board[y][x] == 0) {
        for (int tx = -1; tx <= 1; tx++) {  // Check all 8 directions
            for (int ty = -1; ty <= 1; ty++) {
                if (tx == 0 && ty == 0) continue;  // Skip the current tile
                int nx = x + tx;
                int ny = y + ty;
                flood_reveal(win, board, nx, ny, size, revealed, flags);  // Recursive call
            }
        }
    }
}

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

void print_start(WINDOW *win,int size){
    wclear(win);
    box(win,0,0); 
    wattron(win, COLOR_PAIR(STARTING));
    // remember size is half the window size
    mvwprintw(win, size/2+1, size-8, "[ENTER] TO START");
    wattroff(win, COLOR_PAIR(STARTING));
    wgetch(win);
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

bool check_win(int** board, int size, int** revealed) {
    for (int y = 0; y < size; y++) {
        for (int x = 0; x < size; x++) {
            if (board[y][x] != -1 && revealed[y][x] == 0) {
                return false; // A non-mine cell is still hidden
            }
        }
    }
    return true;
}

void print_status(WINDOW *win, int num_mines, int flags_placed, int size, int state, int click){
    mvwprintw(win, 1, 2, "Mines: %d", num_mines);


    
    if (click == 0){ // normal face
        if (state == -1){ // sad face
            wattron(win, COLOR_PAIR(FLAG));
            mvwprintw(win, 1, size*2-3-1, "T_T");
            wattroff(win, COLOR_PAIR(FLAG));
        }
        // if (state == 1){ // happy face
        //     mvwprintw(win, 1, size*2-6-1, "(✿◠‿◠)");
        // }
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
    mvwprintw(win, 2, 2, "Flags: %d", flags_placed);
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
    init_pair(FLAG, COLOR_YELLOW, COLOR_BLACK);
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

int main() {
    srand(time(NULL));

    const int NUM_MINES = 15;
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
    curs_set(0); // Hide cursor
    int rows, cols;
    getmaxyx(stdscr, rows, cols);  // Get terminal size

    int start_y = (rows - (BOARD_SIZE + 2)) / 2;
    int start_x = (cols - (BOARD_SIZE * 2 + 1)) / 2;

    WINDOW *board_win = newwin(BOARD_SIZE+2, BOARD_SIZE*2+1, start_y, start_x);  // (height, width, start_y, start_x)
    keypad(board_win, TRUE); // Allow arrow key input
    
    int status_height = 5;

    // Create status window below the board
    WINDOW *status_win = newwin(status_height, BOARD_SIZE*2+1, start_y + BOARD_SIZE+2, start_x);
    box(status_win, 0, 0);

    start_color();
    init_colors();

    int ch;
    int curx=BOARD_SIZE/2, cury=BOARD_SIZE/2;
    int flags_placed=0;
    int state = 0; // -1 = lost, 0 = playing, 1 = won

    print_start(board_win, BOARD_SIZE);


    // **Initial draw of the board before the loop**
    print_board(board_win, board, BOARD_SIZE, curx, cury, revealed, flags);

    print_status(status_win, NUM_MINES, flags_placed, BOARD_SIZE, state, 0);

    while ((ch = wgetch(board_win)) != 'q') {  // Use wgetch to get input from the window
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
            case ' ': // Reveal a tile
                if (revealed[cury][curx] == 1) break; // Skip if already revealed

                // do surpised face
                print_status(status_win, NUM_MINES, flags_placed, BOARD_SIZE, state, 1);

                // wiat for a second
                napms(200);

                if (board[cury][curx] == -1){
                    state = -1;
                    print_lost(board_win, board, BOARD_SIZE, revealed, flags);
                }
                if (board[cury][curx] == 0){
                    flood_reveal(board_win, board, curx, cury, BOARD_SIZE, revealed, flags);
                } 
                revealed[cury][curx] = 1;

                if(check_win(board,BOARD_SIZE,revealed) && state == 0){
                    print_win(board_win);
                    state = 1;
                }
                break;
            case 'f': // Place a flag
            case 'x': // Place a flag
                if (revealed[cury][curx] == 1) break; // Skip if already revealed
                flags[cury][curx] = (flags[cury][curx] + 1)%2;
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

        print_board(board_win, board, BOARD_SIZE, curx, cury, revealed, flags); // Now print AFTER movement
    print_status(status_win, NUM_MINES, flags_placed, BOARD_SIZE, state, 0);
    }

    endwin();

    // Free memory
    for (int i = 0; i < BOARD_SIZE; i++){
        free(board[i]);
        free(revealed[i]);
        free(flags[i]);
    }
    free(board);
    free(revealed);
    free(flags);

    return 0;
}
