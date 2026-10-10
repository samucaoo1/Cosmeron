CC ?= gcc
CFLAGS ?= -std=c11 -Wall -Wextra -Wpedantic -Werror -D_POSIX_C_SOURCE=200809L
BUILD ?= build
SOURCES := IncludeAll Terminal Canvas UI UIAdvanced UIGameWidgets
.PHONY: all run clean
all: run
run: $(addprefix $(BUILD)/,$(SOURCES))
	@set -e; $(foreach test,$(SOURCES),$(BUILD)/$(test);)
$(BUILD):
	mkdir -p $(BUILD)
$(BUILD)/%: %.c | $(BUILD)
	$(CC) $(CFLAGS) $< -o $@
clean:
	rm -rf build build-clang build-windows
