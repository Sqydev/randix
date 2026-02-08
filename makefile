# Global settings
MAKEFLAGS += --no-builtin-rules --warn-undefined-variables
.SUFFIXES:

CC ?= gcc

# Flags
BASE_CFLAGS := -Wall -Wextra -Werror
DEV_CFLAGS  := -Og -g3 -fno-omit-frame-pointer -fsanitize=address,undefined
REL_CFLAGS  := -O2

PROFILE ?= local
LIBC    ?= glibc

ifeq ($(LIBC),static-musl)
  LIBC_CFLAGS  := -static
  LIBC_LDFLAGS := -static
else
  LIBC_CFLAGS  :=
  LIBC_LDFLAGS :=
endif

override CFLAGS += $(BASE_CFLAGS) $(REL_CFLAGS) $(LIBC_CFLAGS)
LDFLAGS ?= $(LIBC_LDFLAGS)

# Paths
SRC_DIR := src
OBJ_DIR := obj
BIN_DIR := compiled
TARGET  := randix

OBJ_SUBDIR := $(OBJ_DIR)/$(PROFILE)-$(LIBC)

ifeq ($(filter $(PROFILE),san check),)
  BIN_SUBDIR := $(PROFILE)
else
  BIN_SUBDIR := test
endif

OUT := $(BIN_DIR)/$(BIN_SUBDIR)/$(TARGET)-$(PROFILE)-$(LIBC)

# Sources
SRC := $(sort $(shell find $(SRC_DIR) -name '*.c'))
OBJ := $(patsubst $(SRC_DIR)/%.c,$(OBJ_SUBDIR)/%.o,$(SRC))
DEP := $(OBJ:.o=.d)

.PHONY: all release install build local-build san-build check-build \
        docker-bleeding docker-normal docker-stable \
        docker-bleeding-musl docker-normal-musl docker-stable-musl \
        docker-static-musl clean clean-all

# WAŻNE: Tu musi być wszystko w jednej linii lub poprawnie łamane backslashem
all: local-build san-build check-build docker-bleeding docker-normal docker-stable docker-bleeding-musl docker-normal-musl docker-stable-musl docker-static-musl

release: $(SRC)
	$(CC) $(CFLAGS) $(LDFLAGS) -o randix $(SRC)

PREFIX ?= /usr/local
BINDIR ?= $(PREFIX)/bin

install:
	install -Dm755 randix $(DESTDIR)$(BINDIR)/randix

build: $(OUT)

local-build:
	$(MAKE) PROFILE=local LIBC=glibc CFLAGS="$(BASE_CFLAGS) $(REL_CFLAGS)" LDFLAGS="" build

test-build: san-build check-build

san-build:
	$(MAKE) PROFILE=san LIBC=glibc CFLAGS="$(BASE_CFLAGS) $(DEV_CFLAGS)" LDFLAGS="-fsanitize=address,undefined" build

check-build:
	$(MAKE) PROFILE=check LIBC=glibc build

# Core rules
$(OUT): $(OBJ)
	@mkdir -p $(@D)
	$(CC) $(CFLAGS) $(OBJ) -o $@ $(LDFLAGS)

$(OBJ_SUBDIR)/%.o: $(SRC_DIR)/%.c
	@mkdir -p $(@D)
	$(CC) $(CFLAGS) -MMD -MP -c $< -o $@

-include $(DEP)

# Docker helper variables
UID := $(shell id -u)
GID := $(shell id -g)
DOCKER_USER := --rm -u $(UID):$(GID) -v "$(shell pwd)":/src -w /src
DOCKER_ROOT := --rm -v "$(shell pwd)":/src -w /src
FIX_PERMS   := ; chown -R $(UID):$(GID) $(OBJ_DIR) $(BIN_DIR)

docker-bleeding:
	docker run $(DOCKER_USER) gcc:latest $(MAKE) PROFILE=bleeding LIBC=glibc build

docker-normal:
	docker run $(DOCKER_USER) gcc:11 $(MAKE) PROFILE=normal LIBC=glibc build

docker-stable:
	docker run $(DOCKER_USER) gcc:7 $(MAKE) PROFILE=stable LIBC=glibc build

docker-bleeding-musl:
	docker run $(DOCKER_ROOT) alpine:latest sh -c "apk add --no-cache build-base musl-dev && $(MAKE) PROFILE=bleeding LIBC=musl build $(FIX_PERMS)"

docker-normal-musl:
	docker run $(DOCKER_ROOT) alpine:latest sh -c "apk add --no-cache build-base musl-dev && $(MAKE) PROFILE=normal LIBC=musl build $(FIX_PERMS)"

docker-stable-musl:
	docker run $(DOCKER_ROOT) alpine:3.19 sh -c "apk add --no-cache build-base musl-dev && $(MAKE) PROFILE=stable LIBC=musl build $(FIX_PERMS)"

docker-static-musl:
	docker run $(DOCKER_ROOT) alpine:3.19 sh -c "apk add --no-cache build-base musl-dev && $(MAKE) PROFILE=static LIBC=static-musl build $(FIX_PERMS)"

clean-all:
	rm -rf $(OBJ_DIR) $(BIN_DIR)

clean:
	rm -rf $(OBJ_SUBDIR)
