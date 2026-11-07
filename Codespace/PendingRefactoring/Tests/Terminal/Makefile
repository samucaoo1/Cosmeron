CC ?= gcc
CFLAGS ?= -std=c11 -Wall -Wextra -Wpedantic -Werror -D_POSIX_C_SOURCE=200809L
BUILD ?= build
SOURCES := IncludeAll Rasterization Sprite Curves ProjectionSprites Raycasting Pseudo3DRoad ProjectionCore Terminal Canvas UI UIAdvanced UIGameWidgets UIOverlays Font FIGlet FontCosmeron FontCosmeronBold FontNibbleBits Screen Lifecycle FunctionTable FunctionTableOff TUI UIFunctionTable UIFunctionTableOff Geometry Transform PaletteGradient GraphicsExtensions
.PHONY: all run clean
all: run
run: $(addprefix $(BUILD)/,$(SOURCES))
	@set -e; $(foreach test,$(SOURCES),$(BUILD)/$(test);)
$(BUILD):
	mkdir -p $(BUILD)
$(BUILD)/%: %.c | $(BUILD)
	$(CC) $(CFLAGS) $< -o $@ -lm
clean:
	rm -rf build build-clang build-windows
