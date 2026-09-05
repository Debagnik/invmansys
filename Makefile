CC = gcc
CFLAGS = -Wall -Wextra -Wpedantic -std=c11 -O2

SRC_DIR = src
INC_DIR = include
TOOLS_DIR = tools
BIN_DIR = bin

ifeq ($(OS),Windows_NT)
    EXE = .exe
    CFLAGS += -DPDC_NCMOUSE
    LDFLAGS ?= -lpdcurses_wincon -lwinmm
    MKDIR = if not exist $(BIN_DIR) mkdir $(BIN_DIR)
    RM_OBJS = -del /f /q $(subst /,\,$(OBJS)) 2>nul || exit 0
    RM_BIN = -rmdir /s /q $(BIN_DIR) 2>nul || exit 0
    RUN_CMD = $(TARGET)
else
    EXE =
    LDFLAGS ?= -lncurses
    MKDIR = mkdir -p $(BIN_DIR)
    RM_OBJS = rm -f $(OBJS)
    RM_BIN = rm -rf $(BIN_DIR)
    RUN_CMD = ./$(TARGET)
endif

TARGET = $(BIN_DIR)/invmansys$(EXE)
TOOLS_TARGET = $(BIN_DIR)/debagnik2csv$(EXE) $(BIN_DIR)/csv2debagnik$(EXE)

SRCS = $(SRC_DIR)/main.c $(SRC_DIR)/inventory.c $(SRC_DIR)/storage.c $(SRC_DIR)/tui_ncurses.c
OBJS = $(SRCS:.c=.o)

all: $(BIN_DIR) $(TARGET) $(TOOLS_TARGET)

$(BIN_DIR):
	$(MKDIR)

$(TARGET): $(OBJS) | $(BIN_DIR)
	$(CC) $(CFLAGS) -o $@ $^ $(LDFLAGS)

$(BIN_DIR)/debagnik2csv$(EXE): $(TOOLS_DIR)/debagnik2csv.c $(SRC_DIR)/inventory.c $(SRC_DIR)/storage.c | $(BIN_DIR)
	$(CC) $(CFLAGS) -o $@ $^

$(BIN_DIR)/csv2debagnik$(EXE): $(TOOLS_DIR)/csv2debagnik.c $(SRC_DIR)/inventory.c $(SRC_DIR)/storage.c | $(BIN_DIR)
	$(CC) $(CFLAGS) -o $@ $^

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	$(RM_OBJS)
	$(RM_BIN)

run: $(TARGET)
	$(RUN_CMD)

.PHONY: all clean run

