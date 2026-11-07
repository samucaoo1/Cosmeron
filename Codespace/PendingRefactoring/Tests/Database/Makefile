CC ?= gcc
CFLAGS ?= -std=c11 -Wall -Wextra -Wpedantic -Werror
BUILD ?= build
SOURCES := IncludeAll Database Persistence Namespace SQLiteThreadMode
HEADERS := $(shell find ../../Cosmeron/Modules/Database -type f)
BACKEND_HEADERS := $(shell find ../../Cosmeron/Modules/Database/SQLite -type f)
.PHONY: all run clean
all: run
run: $(addprefix $(BUILD)/,$(SOURCES)) $(BUILD)/MultiTU
	@set -e; $(foreach test,$(SOURCES),$(BUILD)/$(test);) $(BUILD)/MultiTU;
$(BUILD):
	mkdir -p $(BUILD)
$(BUILD)/Backend.o: Backend.c $(BACKEND_HEADERS) | $(BUILD)
	$(CC) $(CFLAGS) -c $< -o $@
$(BUILD)/%: %.c $(BUILD)/Backend.o $(HEADERS) | $(BUILD)
	$(CC) $(CFLAGS) $< $(BUILD)/Backend.o -o $@
$(BUILD)/MultiTU: MultiTU_A.c MultiTU_B.c MultiTU_Main.c $(BUILD)/Backend.o $(HEADERS) | $(BUILD)
	$(CC) $(CFLAGS) MultiTU_A.c MultiTU_B.c MultiTU_Main.c $(BUILD)/Backend.o -o $@
clean:
	rm -rf build build-clang build-sanitize build-windows build-32 .msvc
