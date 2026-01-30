CC      ?= gcc
CFLAGS  := -Wall -Wextra -Werror -O2
LDFLAGS :=

SRC_DIR := src
OBJ_DIR := obj
BIN_DIR := compiled
TARGET  := randix

PROFILE ?= test

SRC := $(shell find $(SRC_DIR) -name '*.c')
OBJ := $(patsubst $(SRC_DIR)/%.c,$(OBJ_DIR)/$(PROFILE)/%.o,$(SRC))
OUT := $(BIN_DIR)/$(PROFILE)/$(TARGET)-$(PROFILE)

.PHONY: all test clean clean-all bleeding normal stable

all:
	$(MAKE) PROFILE=test test
	$(MAKE) PROFILE=bleeding bleeding
	$(MAKE) PROFILE=normal normal
	$(MAKE) PROFILE=stable stable

test: $(OUT)

$(OUT): $(OBJ)
	@mkdir -p $(dir $@)
	$(CC) $(CFLAGS) $(OBJ) -o $@ $(LDFLAGS)

$(OBJ_DIR)/$(PROFILE)/%.o: $(SRC_DIR)/%.c
	@mkdir -p $(dir $@)
	$(CC) $(CFLAGS) -c $< -o $@

bleeding:
	docker run --rm \
	  -v "$(shell pwd)":/src \
	  -w /src \
	  gcc:latest \
	  make PROFILE=bleeding test

normal:
	docker run --rm \
	  -v "$(shell pwd)":/src \
	  -w /src \
	  gcc:11-bullseye \
	  make PROFILE=normal test

stable:
	docker run --rm \
	  -v "$(shell pwd)":/src \
	  -w /src \
	  ubuntu:18.04 \
	  bash -c "apt update && apt install -y build-essential && make PROFILE=stable test"

clean:
	rm -rf $(OBJ_DIR)/$(PROFILE) $(BIN_DIR)/$(PROFILE)

clean-all:
	rm -rf $(OBJ_DIR) $(BIN_DIR)
