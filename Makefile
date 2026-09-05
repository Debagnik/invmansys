CC = gcc
CFLAGS = -Wall -Wextra -Wpedantic -std=c11 -O2
LDFLAGS = -lncurses

SRC_DIR = src
INC_DIR = include
TOOLS_DIR = tools
BIN_DIR = bin

TARGET = $(BIN_DIR)/invmansys
TOOLS_TARGET = $(BIN_DIR)/debagnik2csv $(BIN_DIR)/csv2debagnik

SRCS = $(SRC_DIR)/main.c $(SRC_DIR)/inventory.c $(SRC_DIR)/storage.c $(SRC_DIR)/tui_ncurses.c
OBJS = $(SRCS:.c=.o)

all: $(BIN_DIR) $(TARGET) $(TOOLS_TARGET)

$(BIN_DIR):
	mkdir -p $(BIN_DIR)

$(TARGET): $(OBJS) | $(BIN_DIR)
	$(CC) $(CFLAGS) -o $@ $^ $(LDFLAGS)

$(BIN_DIR)/debagnik2csv: $(TOOLS_DIR)/debagnik2csv.c $(SRC_DIR)/inventory.c $(SRC_DIR)/storage.c | $(BIN_DIR)
	$(CC) $(CFLAGS) -o $@ $^

$(BIN_DIR)/csv2debagnik: $(TOOLS_DIR)/csv2debagnik.c $(SRC_DIR)/inventory.c $(SRC_DIR)/storage.c | $(BIN_DIR)
	$(CC) $(CFLAGS) -o $@ $^

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -f $(OBJS)
	rm -rf $(BIN_DIR)

run: $(TARGET)
	./$(TARGET)

.PHONY: all clean run
