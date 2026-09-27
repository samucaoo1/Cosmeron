CC ?= cc
CFLAGS ?= -std=c11 -Wall -Wextra -Wpedantic -Werror -D_POSIX_C_SOURCE=200809L
BUILD ?= .build
LDLIBS ?= 

TESTS := Random Engines Distribution IncludeAll

all: run

$(BUILD):
	mkdir -p $(BUILD)

$(BUILD)/%: %.c | $(BUILD)
	$(CC) $(CFLAGS) $< -o $@ $(LDLIBS)

build: $(TESTS:%=$(BUILD)/%)

run: build
	@set -e; for test in $(TESTS:%=$(BUILD)/%); do echo "$$test"; "$$test"; done

clean:
	rm -rf $(BUILD)

.PHONY: all build run clean
