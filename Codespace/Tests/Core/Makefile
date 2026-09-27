CC ?= cc
CFLAGS ?= -std=c11 -Wall -Wextra -Wpedantic -Werror -D_POSIX_C_SOURCE=200809L
BUILD ?= .build

TESTS := Namespace Cast Preprocessor Detect Error Memory MemoryOperations MemoryUtilities Algorithm IncludeAll

all: run

$(BUILD):
	mkdir -p $(BUILD)

$(BUILD)/%: %.c | $(BUILD)
	$(CC) $(CFLAGS) $< -o $@

build: $(TESTS:%=$(BUILD)/%)

run: build
	@for test in $(TESTS:%=$(BUILD)/%); do echo "$$test"; "$$test"; done

clean:
	rm -rf $(BUILD)

.PHONY: all build run clean
