CC ?= gcc
CFLAGS ?= -std=c11 -Wall -Wextra -Wpedantic -Werror -D_POSIX_C_SOURCE=200809L
BUILD ?= build
SOURCES := Logging IncludeAll Namespace FunctionTableOff

.PHONY: all run clean

all: run

run: $(addprefix $(BUILD)/,$(SOURCES)) $(BUILD)/MultiTU
	@set -e; $(foreach test,$(SOURCES),$(BUILD)/$(test);) $(BUILD)/MultiTU;

$(BUILD):
	mkdir -p $(BUILD)

HEADERS := $(wildcard ../../Cosmeron/Modules/Logging/*.space ../../Cosmeron/Modules/Logging/*.h ../../Cosmeron/Modules/Logging/*/*.h ../../Cosmeron/Modules/Logging/*/Impl/*.impl ../../Cosmeron/Modules/Logging/FunctionTable/*.inc)

$(BUILD)/%: %.c $(HEADERS) | $(BUILD)
	$(CC) $(CFLAGS) $< -o $@

$(BUILD)/MultiTU: MultiTU_A.c MultiTU_B.c MultiTU_Main.c $(HEADERS) | $(BUILD)
	$(CC) $(CFLAGS) $(filter %.c,$^) -o $@

clean:
	rm -rf $(BUILD)
