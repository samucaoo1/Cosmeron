CC ?= cc
CFLAGS ?= -std=c11 -Wall -Wextra -Wpedantic -Werror
BUILD ?= .build
TESTS := State Application Namespace Linux Windows MacOS SDLControllers IncludeAll
HEADERS := $(shell find ../../Cosmeron/Modules/Input ../../Cosmeron/Core -type f)
all: run
$(BUILD):
	mkdir -p $(BUILD)
$(BUILD)/%: %.c $(HEADERS) | $(BUILD)
	$(CC) $(CFLAGS) $< -o $@
$(BUILD)/MultiTU: MultiTU_A.c MultiTU_B.c MultiTU_Main.c $(HEADERS) | $(BUILD)
	$(CC) $(CFLAGS) MultiTU_A.c MultiTU_B.c MultiTU_Main.c -o $@
$(BUILD)/ExternalStorage: MultiTU_A.c MultiTU_B.c MultiTU_Main.c $(HEADERS) | $(BUILD)
	$(CC) $(CFLAGS) -DINPUT_EXTERNAL_STORAGE MultiTU_A.c MultiTU_B.c MultiTU_Main.c -o $@
build: $(TESTS:%=$(BUILD)/%) $(BUILD)/MultiTU $(BUILD)/ExternalStorage
run: build
	@set -e; for test in $(TESTS:%=$(BUILD)/%) $(BUILD)/MultiTU $(BUILD)/ExternalStorage; do echo "Running $$test"; "$$test"; done
clean:
	rm -rf $(BUILD)
.PHONY: all build run clean
