CC ?= cc
CFLAGS ?= -std=c11 -Wall -Wextra -Wpedantic -Werror
BUILD ?= .build
TESTS := Synthesis Codec Player IncludeAll
HEADERS := $(shell find ../../Cosmeron/Modules/Audio ../../Cosmeron/Core -type f)

all: run
$(BUILD):
	mkdir -p $(BUILD)
$(BUILD)/%: %.c $(HEADERS) | $(BUILD)
	$(CC) $(CFLAGS) $< -o $@
$(BUILD)/MultiTU: MultiTU_A.c MultiTU_B.c MultiTU_Main.c $(HEADERS) | $(BUILD)
	$(CC) $(CFLAGS) MultiTU_A.c MultiTU_B.c MultiTU_Main.c -o $@
build: $(TESTS:%=$(BUILD)/%) $(BUILD)/MultiTU
run: build
	@set -e; for test in $(TESTS:%=$(BUILD)/%) $(BUILD)/MultiTU; do "$$test"; done
clean:
	rm -rf $(BUILD) roundtrip.wav
.PHONY: all build run clean
