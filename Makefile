CC = gcc -g
CFLAGS = -Wall -Wextra -Iinclude -pthread
SRC_DIR = src
OBJ_DIR = obj
BIN = chttpd

# Use find to get all .c files in src/ and its subdirectories
SRCS = $(shell find $(SRC_DIR) -name "*.c")
# Map SRCS to OBJS in obj/ directory, keeping the same subdirectory structure
OBJS = $(SRCS:$(SRC_DIR)/%.c=$(OBJ_DIR)/%.o)

all: $(BIN)

$(BIN): $(OBJS)
	$(CC) $(CFLAGS) -o $@ $^

# Rule to create object files, ensures the target directory exists
$(OBJ_DIR)/%.o: $(SRC_DIR)/%.c
	@mkdir -p $(dir $@)
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -rf $(OBJ_DIR) $(BIN)

.PHONY: all clean
