# Minesweeper in C

This is a command-line Minesweeper game written in C using `ncurses`. This was my first project in C, created as a way to learn the language, memory management, and terminal-based UI development.

## Features

- **Classic Minesweeper gameplay** with random mine placement
- **Ncurses-based UI** for smooth terminal interaction
- **Flood reveal** for uncovering multiple empty cells
- **Flagging system** to mark potential mines
- **Win/Loss detection**
- **Customizable board size and mine count**

## Installation

To compile and run the game, ensure you have `ncurses` installed.

### Linux (Debian-based distros)

```sh
sudo apt install libncurses5-dev libncursesw5-dev
```

### MacOS (using Homebrew)

```sh
brew install ncurses
```

### Compilation

To compile using `make`, run:

```sh
make
```

This will generate an executable named `mine`.

To remove the compiled binary, use:

```sh
make clean
```

### Running the Game

```sh
./mine
```

## How to Play

- Use **arrow keys** to move the cursor
- Press **Enter** to reveal a tile
- Press **F** to place/remove a flag
- The goal is to reveal all non-mine tiles without detonating a mine

## Future Improvements

- Add difficulty selection at startup
- Implement a timer and score tracking
- Improve UI with color themes

## License

This project is open-source under the MIT License.

