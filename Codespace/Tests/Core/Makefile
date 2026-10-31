CC ?= cc
CFLAGS ?= -std=c11 -Wall -Wextra -Wpedantic -Werror -D_POSIX_C_SOURCE=200809L
BUILD ?= .build

TESTS := Namespace Cast Primitive Preprocessor Detect Error Memory MemoryOperations MemoryUtilities Stream Algorithm Callback Object ObjectEx ObjectNoFunctionTable Package PackageNoFunctionTable Pool PoolNoFunctionTable Calculator CalculatorNoFunctionTable IncludeAll

all: run

$(BUILD):
	mkdir -p $(BUILD)

$(BUILD)/%: %.c | $(BUILD)
	$(CC) $(CFLAGS) $< -o $@

build: $(TESTS:%=$(BUILD)/%)

run: build
	@for test in $(TESTS:%=$(BUILD)/%); do echo "$$test"; "$$test"; done

clean:
	rm -rf $(BUILD)

.PHONY: all build run clean
