CC ?= gcc
CFLAGS := -std=c11 -Wall -Wextra -Wpedantic -Werror -D_POSIX_C_SOURCE=200809L
BUILD := build
TESTS := include-all text namespace unicode all-functions

.PHONY: all strict gcc clang run clean
all: strict
strict: gcc clang
gcc:
	$(MAKE) CC=gcc BUILD=build run
clang:
	$(MAKE) CC=clang BUILD=build-clang run
run: $(addprefix $(BUILD)/,$(TESTS))
	@set -e; $(foreach test,$(TESTS),$(BUILD)/$(test);)
$(BUILD):
	mkdir -p $(BUILD)
$(BUILD)/include-all: IncludeAll.c | $(BUILD)
	$(CC) $(CFLAGS) $< -o $@
$(BUILD)/text: Text.c | $(BUILD)
	$(CC) $(CFLAGS) $< -o $@
$(BUILD)/namespace: Namespace.c | $(BUILD)
	$(CC) $(CFLAGS) $< -o $@
$(BUILD)/unicode: Unicode.c | $(BUILD)
	$(CC) $(CFLAGS) $< -o $@
$(BUILD)/all-functions: AllFunctions.c | $(BUILD)
	$(CC) $(CFLAGS) $< -o $@
clean:
	rm -rf build build-clang
