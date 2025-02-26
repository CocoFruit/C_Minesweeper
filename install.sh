#!/bin/bash

# Define paths
EXEC_NAME="mine"
BIN_PATH="bin/$EXEC_NAME"
INSTALL_DIR="$HOME/.local/bin"

# Check if Makefile exists
if [ ! -f "Makefile" ]; then
    echo "Makefile not found! Please run this script from the project directory."
    exit 1
fi

# Compile the program
echo "Compiling $EXEC_NAME..."
make

# Check if compilation was successful
if [ ! -f "$BIN_PATH" ]; then
    echo "Compilation failed! Make sure your Makefile produces an executable at '$BIN_PATH'."
    exit 1
fi

# Ensure install directory exists
mkdir -p "$INSTALL_DIR"

# Move the executable
echo "Moving $BIN_PATH to $INSTALL_DIR/$EXEC_NAME..."
cp "$BIN_PATH" "$INSTALL_DIR/$EXEC_NAME"

# Ensure ~/.local/bin is in PATH
if [[ ":$PATH:" != *":$INSTALL_DIR:"* ]]; then
    echo "Adding $INSTALL_DIR to PATH..."
    echo 'export PATH=$HOME/.local/bin:$PATH' >> ~/.bashrc
    echo 'export PATH=$HOME/.local/bin:$PATH' >> ~/.zshrc
    export PATH=$HOME/.local/bin:$PATH
fi

echo "Installation complete! You can now run '$EXEC_NAME' from anywhere."
