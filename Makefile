CC = gcc
CFLAGS = `pkg-config --cflags gtk+-2.0`
LIBS = `pkg-config --libs gtk+-2.0`
WARNINGS = -Wno-deprecated-declarations

SRC_DIR = src
INC_DIR = include
OBJ_DIR = obj
OUT_DIR = out

SRCS = $(wildcard $(SRC_DIR)/*.c)
OBJS = $(patsubst $(SRC_DIR)/%.c, $(OBJ_DIR)/%.o, $(SRCS))
EXEC = $(OUT_DIR)/main

.PHONY: all clean

all: $(EXEC)

$(EXEC): $(OBJS)
	$(CC) $(CFLAGS) $(OBJS) -o $(EXEC) $(LIBS) $(WARNINGS)

$(OBJ_DIR)/%.o: $(SRC_DIR)/%.c
	$(CC) $(CFLAGS) -c $< -o $@ $(WARNINGS) -I$(INC_DIR)

clean:
	rm -f $(OBJS) $(EXEC)