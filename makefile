# Global Settings

MAKEFLAGS += --no-builtin-rules --warn-undefined-variables
.SUFFIXES:

CC ?= gcc
ifneq ($(shell command -v ccache 2>/dev/null),)
  CC := ccache $(CC)
endif

BASE_CFLAGS := -Wall -Wextra -Werror
DEV_CFLAGS  := -Og -g3 -fno-omit-frame-pointer \
               -fsanitize=address,undefined
REL_CFLAGS  := -O2

BASE_LDFLAGS :=
DEV_LDFLAGS  := -fsanitize=address,undefined
REL_LDFLAGS  :=

PROFILE ?= local
LIBC    ?= glibc   # glibc | musl | static-musl

# libc-specific flags
ifeq ($(LIBC),static-musl)
  LIBC_CFLAGS  := -static
  LIBC_LDFLAGS := -static
else
  LIBC_CFLAGS  :=
  LIBC_LDFLAGS :=
endif

CFLAGS  ?= $(BASE_CFLAGS) $(REL_CFLAGS) $(LIBC_CFLAGS)
LDFLAGS ?= $(BASE_LDFLAGS) $(REL_LDFLAGS) $(LIBC_LDFLAGS)

SRC_DIR := src
OBJ_DIR := obj
BIN_DIR := compiled
TARGET  := randix

# Profiles that go into compiled/test
TEST_PROFILES := san check

ifeq ($(filter $(PROFILE),$(TEST_PROFILES)),)
  BIN_SUBDIR := $(PROFILE)
else
  BIN_SUBDIR := test
endif

SRC := $(sort $(wildcard $(SRC_DIR)/**/*.c) $(wildcard $(SRC_DIR)/*.c))
OBJ := $(patsubst $(SRC_DIR)/%.c,$(OBJ_DIR)/$(PROFILE)-$(LIBC)/%.o,$(SRC))
OUT := $(BIN_DIR)/$(BIN_SUBDIR)/$(TARGET)-$(PROFILE)-$(LIBC)

# Phony 

.PHONY: all build local san check test-build \
        bleeding bleeding-musl \
        normal normal-musl \
        stable stable-musl \
        static-musl \
        clean clean-all

all: local test-build \
     bleeding bleeding-musl \
     normal normal-musl \
     stable stable-musl \
     static-musl

# Local / Test Targets 

build: $(OUT)

local:
	$(MAKE) PROFILE=local LIBC=glibc \
	    CFLAGS="$(BASE_CFLAGS) $(REL_CFLAGS)" \
	    LDFLAGS="$(BASE_LDFLAGS) $(REL_LDFLAGS)" \
	    build

san:
	$(MAKE) PROFILE=san LIBC=glibc \
	    CFLAGS="$(BASE_CFLAGS) $(DEV_CFLAGS)" \
	    LDFLAGS="$(BASE_LDFLAGS) $(DEV_LDFLAGS)" \
	    build

check:
	$(MAKE) PROFILE=check LIBC=$(LIBC) \
	    CFLAGS="$(CFLAGS)" \
	    LDFLAGS="$(LDFLAGS)" \
	    build

# Meta target: sanitizer + check
test-build: san check

# Core Build 

$(OUT): $(OBJ)
	@mkdir -p $(dir $@)
	$(CC) $(CFLAGS) $(OBJ) -o $@ $(LDFLAGS)

$(OBJ_DIR)/$(PROFILE)-$(LIBC)/%.o: $(SRC_DIR)/%.c
	@mkdir -p $(dir $@)
	$(CC) $(CFLAGS) -MMD -MP -c $< -o $@

-include $(OBJ:.o=.d)

# Glibc (Docker)

bleeding:
	docker run --rm -v "$(shell pwd)":/src -w /src \
	  gcc:latest \
	  make PROFILE=bleeding LIBC=glibc build

normal:
	docker run --rm -v "$(shell pwd)":/src -w /src \
	  gcc:11 \
	  make PROFILE=normal LIBC=glibc build

stable:
	docker run --rm -v "$(shell pwd)":/src -w /src \
	  ubuntu:18.04 \
	  bash -c "apt update && apt install -y build-essential && \
	           make PROFILE=stable LIBC=glibc build"

# Musl (Docker, Dynamic)

bleeding-musl:
	docker run --rm -v "$(shell pwd)":/src -w /src \
	  alpine:latest \
	  sh -c "apk add --no-cache build-base musl-dev && \
	         make PROFILE=bleeding LIBC=musl build"

normal-musl:
	docker run --rm -v "$(shell pwd)":/src -w /src \
	  alpine:latest \
	  sh -c "apk add --no-cache build-base musl-dev && \
	         make PROFILE=normal LIBC=musl build"

stable-musl:
	docker run --rm -v "$(shell pwd)":/src -w /src \
	  alpine:3.19 \
	  sh -c "apk add --no-cache build-base musl-dev && \
	         make PROFILE=stable LIBC=musl build"

# Musl (Docker, Static)

static-musl:
	docker run --rm -v "$(shell pwd)":/src -w /src \
	  alpine:3.19 \
	  sh -c "apk add --no-cache build-base musl-dev && \
	         make PROFILE=static LIBC=static-musl build"

# Cleanup

clean:
	rm -rf $(OBJ_DIR)/$(PROFILE)-$(LIBC)
	rm -rf $(BIN_DIR)/$(PROFILE) $(BIN_DIR)/test

clean-all:
	rm -rf $(OBJ_DIR) $(BIN_DIR)
