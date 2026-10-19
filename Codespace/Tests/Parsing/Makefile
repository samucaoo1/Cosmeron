CC ?= gcc
CFLAGS ?= -std=c11 -Wall -Wextra -Wpedantic -Werror
BUILD ?= build
SOURCES := Common Cursor JSON INI CSV TOML YAML XML IncludeAll Namespace FunctionTableOff Robustness Allocation
.PHONY: all run clean
all: run
run: $(addprefix $(BUILD)/,$(SOURCES)) $(BUILD)/MultiTU
	@set -e; $(foreach test,$(SOURCES),$(BUILD)/$(test);) $(BUILD)/MultiTU;
$(BUILD):
	mkdir -p $(BUILD)
HEADERS := $(wildcard ../../Cosmeron/Modules/CLI/*.space ../../Cosmeron/Modules/CLI/*/*.h ../../Cosmeron/Modules/CLI/*/Impl/*.impl ../../Cosmeron/Modules/CLI/FunctionTable/*.inc ../../Cosmeron/Modules/Parsing/*.space ../../Cosmeron/Modules/Parsing/*/*.h ../../Cosmeron/Modules/Parsing/*/Impl/*.impl ../../Cosmeron/Modules/Parsing/FunctionTable/*.inc)
$(BUILD)/%: %.c $(HEADERS) | $(BUILD)
	$(CC) $(CFLAGS) $< -o $@
$(BUILD)/MultiTU: MultiTU_A.c MultiTU_B.c MultiTU_Main.c $(HEADERS) | $(BUILD)
	$(CC) $(CFLAGS) $(filter %.c,$^) -o $@
clean:
	rm -rf build
