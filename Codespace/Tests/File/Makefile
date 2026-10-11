CC ?= gcc
CFLAGS ?= -std=c11 -Wall -Wextra -Wpedantic -Werror
BUILD ?= build
SOURCES := IncludeAll Path File Namespace
.PHONY: all run clean
all: run
run: $(addprefix $(BUILD)/,$(SOURCES)) $(BUILD)/MultiTU
	@set -e; $(foreach test,$(SOURCES),$(BUILD)/$(test);) $(BUILD)/MultiTU;
$(BUILD):
	mkdir -p $(BUILD)
$(BUILD)/%: %.c | $(BUILD)
	$(CC) $(CFLAGS) $< -o $@
clean:
	rm -rf build build-clang build-sanitize build-windows

$(BUILD)/MultiTU: MultiTU_A.c MultiTU_B.c MultiTU_Main.c | $(BUILD)
	$(CC) $(CFLAGS) $^ -o $@
