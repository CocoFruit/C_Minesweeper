# Compiler settings
CC = gcc
CFLAGS = -Wall -Wextra -pedantic -std=c99 -Iinclude  # Updated to use include/
LDFLAGS = -lncurses

# Directories
SRC_DIR = src
BUILD_DIR = build
BIN_DIR = bin

# Target binary
TARGET = $(BIN_DIR)/mine

# Source and object files
SRCS = $(SRC_DIR)/main.c $(SRC_DIR)/ui.c $(SRC_DIR)/game_logic.c
OBJS = $(patsubst $(SRC_DIR)/%.c, $(BUILD_DIR)/%.o, $(SRCS))

# Build the final binary
$(TARGET): $(OBJS) | $(BIN_DIR)
	$(CC) $(CFLAGS) $(OBJS) -o $(TARGET) $(LDFLAGS)

# Rule for compiling .c files to .o files
$(BUILD_DIR)/%.o: $(SRC_DIR)/%.c | $(BUILD_DIR)
	$(CC) $(CFLAGS) -c $< -o $@

# Ensure directories exist
$(BIN_DIR) $(BUILD_DIR):
	mkdir -p $@

# Clean up object files and the binary
clean:
	rm -rf $(BUILD_DIR) $(TARGET)

# Run the game
run: $(TARGET)
	./$(TARGET)

# Phony targets
.PHONY: clean run
