CC ?= gcc
CFLAGS := -std=c11 -Wall -Wextra -Wpedantic -Werror -D_POSIX_C_SOURCE=200809L
BUILD := build
ASAN_OPTIONS ?= detect_leaks=1

.PHONY: all strict sanitize multi-tu clean

all: strict sanitize multi-tu

strict: $(BUILD)/functional $(BUILD)/linear-extra $(BUILD)/linear-all-types $(BUILD)/tree-all-functions $(BUILD)/btree $(BUILD)/hash $(BUILD)/graph $(BUILD)/off $(BUILD)/tree-ft-off $(BUILD)/custom-type $(BUILD)/include-all $(BUILD)/include-all-off $(BUILD)/ring $(BUILD)/heap $(BUILD)/span
	$(BUILD)/functional
	$(BUILD)/linear-extra
	$(BUILD)/linear-all-types
	$(BUILD)/tree-all-functions
	$(BUILD)/btree
	$(BUILD)/hash
	$(BUILD)/graph
	$(BUILD)/off
	$(BUILD)/tree-ft-off
	$(BUILD)/custom-type
	$(BUILD)/include-all
	$(BUILD)/include-all-off
	$(BUILD)/ring
	$(BUILD)/heap
	$(BUILD)/span

sanitize: CFLAGS += -O1 -g -fsanitize=address,undefined -fno-omit-frame-pointer
sanitize: $(BUILD)/functional-sanitize $(BUILD)/btree-sanitize $(BUILD)/hash-sanitize $(BUILD)/graph-sanitize $(BUILD)/ring-sanitize $(BUILD)/heap-sanitize $(BUILD)/span-sanitize
	ASAN_OPTIONS=$(ASAN_OPTIONS) $(BUILD)/functional-sanitize
	ASAN_OPTIONS=$(ASAN_OPTIONS) $(BUILD)/btree-sanitize
	ASAN_OPTIONS=$(ASAN_OPTIONS) $(BUILD)/hash-sanitize
	ASAN_OPTIONS=$(ASAN_OPTIONS) $(BUILD)/graph-sanitize
	ASAN_OPTIONS=$(ASAN_OPTIONS) $(BUILD)/ring-sanitize
	ASAN_OPTIONS=$(ASAN_OPTIONS) $(BUILD)/heap-sanitize
	ASAN_OPTIONS=$(ASAN_OPTIONS) $(BUILD)/span-sanitize

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

$(BUILD)/btree: BTree.c Test.h | $(BUILD)
	$(CC) $(CFLAGS) BTree.c -o $@

$(BUILD)/hash: Hash.c Test.h | $(BUILD)
	$(CC) $(CFLAGS) Hash.c -o $@

$(BUILD)/graph: Graph.c Test.h | $(BUILD)
	$(CC) $(CFLAGS) Graph.c -o $@

$(BUILD)/off: FunctionTableOff.c Test.h | $(BUILD)
	$(CC) $(CFLAGS) FunctionTableOff.c -o $@

$(BUILD)/tree-ft-off: TreeFunctionTableOff.c Test.h | $(BUILD)
	$(CC) $(CFLAGS) TreeFunctionTableOff.c -o $@

$(BUILD)/custom-type: CustomType.c Test.h | $(BUILD)
	$(CC) $(CFLAGS) CustomType.c -o $@

$(BUILD)/ring: Ring.c Test.h | $(BUILD)
	$(CC) $(CFLAGS) Ring.c -o $@

$(BUILD)/heap: Heap.c Test.h | $(BUILD)
	$(CC) $(CFLAGS) Heap.c -o $@

$(BUILD)/span: Span.c Test.h | $(BUILD)
	$(CC) $(CFLAGS) Span.c -o $@

$(BUILD)/include-all: IncludeAll.c | $(BUILD)
	$(CC) $(CFLAGS) IncludeAll.c -o $@

$(BUILD)/include-all-off: IncludeAllOff.c IncludeAll.c | $(BUILD)
	$(CC) $(CFLAGS) IncludeAllOff.c -o $@

$(BUILD)/functional-sanitize: Functional.c Test.h | $(BUILD)
	$(CC) $(CFLAGS) Functional.c -o $@

$(BUILD)/btree-sanitize: BTree.c Test.h | $(BUILD)
	$(CC) $(CFLAGS) BTree.c -o $@

$(BUILD)/hash-sanitize: Hash.c Test.h | $(BUILD)
	$(CC) $(CFLAGS) Hash.c -o $@

$(BUILD)/graph-sanitize: Graph.c Test.h | $(BUILD)
	$(CC) $(CFLAGS) Graph.c -o $@

$(BUILD)/ring-sanitize: Ring.c Test.h | $(BUILD)
	$(CC) $(CFLAGS) Ring.c -o $@

$(BUILD)/heap-sanitize: Heap.c Test.h | $(BUILD)
	$(CC) $(CFLAGS) Heap.c -o $@

$(BUILD)/span-sanitize: Span.c Test.h | $(BUILD)
	$(CC) $(CFLAGS) Span.c -o $@

$(BUILD)/multi-tu: MultiTU_A.c MultiTU_B.c MultiTU_Main.c | $(BUILD)
	$(CC) $(CFLAGS) MultiTU_A.c MultiTU_B.c MultiTU_Main.c -o $@

clean:
	rm -rf $(BUILD)
