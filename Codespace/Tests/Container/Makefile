CC ?= gcc
CFLAGS := -std=c11 -Wall -Wextra -Wpedantic -Werror -D_POSIX_C_SOURCE=200809L
BUILD := build
ASAN_OPTIONS ?= detect_leaks=1

.PHONY: all strict sanitize multi-tu clean

all: strict sanitize multi-tu

strict: $(BUILD)/functional $(BUILD)/linear-extra $(BUILD)/linear-all-types $(BUILD)/tree-all-functions $(BUILD)/hash $(BUILD)/graph $(BUILD)/off $(BUILD)/custom-type $(BUILD)/include-all $(BUILD)/include-all-off
	$(BUILD)/functional
	$(BUILD)/linear-extra
	$(BUILD)/linear-all-types
	$(BUILD)/tree-all-functions
	$(BUILD)/hash
	$(BUILD)/graph
	$(BUILD)/off
	$(BUILD)/custom-type
	$(BUILD)/include-all
	$(BUILD)/include-all-off

sanitize: CFLAGS += -O1 -g -fsanitize=address,undefined -fno-omit-frame-pointer
sanitize: $(BUILD)/functional-sanitize $(BUILD)/hash-sanitize $(BUILD)/graph-sanitize
	ASAN_OPTIONS=$(ASAN_OPTIONS) $(BUILD)/functional-sanitize
	ASAN_OPTIONS=$(ASAN_OPTIONS) $(BUILD)/hash-sanitize
	ASAN_OPTIONS=$(ASAN_OPTIONS) $(BUILD)/graph-sanitize

multi-tu: $(BUILD)/multi-tu
	$(BUILD)/multi-tu

$(BUILD):
	mkdir -p $(BUILD)

$(BUILD)/functional: Functional.c Test.h | $(BUILD)
	$(CC) $(CFLAGS) Functional.c -o $@

$(BUILD)/linear-extra: LinearExtra.c Test.h | $(BUILD)
	$(CC) $(CFLAGS) LinearExtra.c -o $@

$(BUILD)/linear-all-types: LinearAllTypes.c Test.h | $(BUILD)
	$(CC) $(CFLAGS) LinearAllTypes.c -o $@

$(BUILD)/tree-all-functions: TreeAllFunctions.c Test.h | $(BUILD)
	$(CC) $(CFLAGS) TreeAllFunctions.c -o $@

$(BUILD)/hash: Hash.c Test.h | $(BUILD)
	$(CC) $(CFLAGS) Hash.c -o $@

$(BUILD)/graph: Graph.c Test.h | $(BUILD)
	$(CC) $(CFLAGS) Graph.c -o $@

$(BUILD)/off: PseudoMethodsOff.c Test.h | $(BUILD)
	$(CC) $(CFLAGS) PseudoMethodsOff.c -o $@

$(BUILD)/custom-type: CustomType.c Test.h | $(BUILD)
	$(CC) $(CFLAGS) CustomType.c -o $@

$(BUILD)/include-all: IncludeAll.c | $(BUILD)
	$(CC) $(CFLAGS) IncludeAll.c -o $@

$(BUILD)/include-all-off: IncludeAllOff.c IncludeAll.c | $(BUILD)
	$(CC) $(CFLAGS) IncludeAllOff.c -o $@

$(BUILD)/functional-sanitize: Functional.c Test.h | $(BUILD)
	$(CC) $(CFLAGS) Functional.c -o $@

$(BUILD)/hash-sanitize: Hash.c Test.h | $(BUILD)
	$(CC) $(CFLAGS) Hash.c -o $@

$(BUILD)/graph-sanitize: Graph.c Test.h | $(BUILD)
	$(CC) $(CFLAGS) Graph.c -o $@

$(BUILD)/multi-tu: MultiTU_A.c MultiTU_B.c MultiTU_Main.c | $(BUILD)
	$(CC) $(CFLAGS) MultiTU_A.c MultiTU_B.c MultiTU_Main.c -o $@

clean:
	rm -rf $(BUILD)
