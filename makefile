CC      := gcc
CFLAGS  := -Wall -Wextra -O2
SRC_DIR := src
BIN_DIR := compiled
TARGET  := randix

SRC := $(SRC_DIR)/main.c
OUT := $(BIN_DIR)/$(TARGET)

.PHONY: linux clean

linux: $(OUT)

$(OUT): $(SRC)
	@mkdir -p $(BIN_DIR)
	$(CC) $(CFLAGS) $^ -o $@

clean:
	rm -rf $(BIN_DIR)/*
