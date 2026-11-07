CC ?= gcc
CFLAGS ?= -std=c11 -Wall -Wextra -Wpedantic -Werror
BUILD ?= build
SOURCES := IncludeAll Codec Zip Namespace FunctionTableOff Robustness Allocation File NativeInflate NativeCodec
HEADERS := $(shell find ../../Cosmeron/Modules/Compression -type f)
.PHONY: all run clean interop zip64
all: run
run: $(addprefix $(BUILD)/,$(SOURCES)) $(BUILD)/MultiTU
	@set -e; $(foreach test,$(SOURCES),$(BUILD)/$(test);) $(BUILD)/MultiTU;
$(BUILD):
	mkdir -p $(BUILD)
$(BUILD)/%: %.c $(HEADERS) | $(BUILD)
	$(CC) $(CFLAGS) $< -o $@
$(BUILD)/MultiTU: MultiTU_A.c MultiTU_B.c MultiTU_Main.c $(HEADERS) | $(BUILD)
	$(CC) $(CFLAGS) $(filter %.c,$^) -o $@
clean:
	rm -rf $(BUILD)

interop: $(BUILD)/Interop
	python3 interop.py $(BUILD)/Interop

zip64: $(BUILD)/Zip64 $(BUILD)/Interop
	python3 zip64.py $(BUILD)/Zip64
