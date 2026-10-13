CC ?= cc
CFLAGS ?= -std=c11 -Wall -Wextra -Wpedantic -Werror -D_POSIX_C_SOURCE=200809L
BUILD ?= .build
TESTS := Surface Interaction Editor Render Containers Failure InputText Window WindowNative Advanced IncludeAll Namespace FunctionTableOff
HEADERS := $(shell find ../../Cosmeron/Modules/Graphics ../../Cosmeron/Modules/Input ../../Cosmeron/Core -type f)
.PHONY: all run build lab clean
all: run
run: build lab
	@set -e; for test in $(TESTS); do $(BUILD)/$$test; done
build: $(TESTS:%=$(BUILD)/%) $(BUILD)/MultiTU
	@$(BUILD)/MultiTU
lab: $(BUILD)/Lab
	@$(BUILD)/Lab --ppm $(BUILD)/Lab.ppm
$(BUILD)/Lab: ../../Examples/GUI/Lab.c $(HEADERS) | $(BUILD)
	$(CC) $(CFLAGS) $< -o $@
$(BUILD):
	mkdir -p $(BUILD)
$(BUILD)/%: %.c Test.h $(HEADERS) | $(BUILD)
	$(CC) $(CFLAGS) $< -o $@
$(BUILD)/MultiTU: MultiTU_A.c MultiTU_B.c MultiTU_Main.c $(HEADERS) | $(BUILD)
	$(CC) $(CFLAGS) MultiTU_A.c MultiTU_B.c MultiTU_Main.c -o $@
clean:
	rm -rf $(BUILD)
