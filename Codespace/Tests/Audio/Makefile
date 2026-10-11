CC ?= cc
CFLAGS ?= -std=c11 -Wall -Wextra -Wpedantic -Werror
BUILD ?= .build
TESTS := Synthesis OfflineStandalone OfflineCompat Manual ManualSink NativeWAV NativeWAVStream WAVRobustness FLACMetadata FLACFrames FLACPublic FLACStandalone FLACRobustness FLACStreaming MP3Headers MP3SideInfo MP3Scalefactors MP3Huffman MP3Requant MP3Hybrid MP3IMDCT MP3Synthesis MP3Intensity MP3Xing MP3Experimental MP3ExperimentalCompare WAVFormats CodecMinimal CodecMath CodecCompressed CodecStandalone DesktopStandalone Codec Player IncludeAll
HEADERS := $(shell find ../../Cosmeron/Modules/Audio ../../Cosmeron/Core -type f)

all: run
$(BUILD):
	mkdir -p $(BUILD)
$(BUILD)/%: %.c $(HEADERS) | $(BUILD)
	$(CC) $(CFLAGS) $< -o $@
# Physical-device helper: compiled as part of build, never in automatic run.
$(BUILD)/ManualHardware: ManualHardware.c $(HEADERS) | $(BUILD)
	$(CC) $(CFLAGS) $< -o $@
$(BUILD)/CodecMinimalMultiTU: CodecMinimalTU_A.c CodecMinimalTU_B.c CodecMinimalTU_Main.c $(HEADERS) | $(BUILD)
	$(CC) $(CFLAGS) CodecMinimalTU_A.c CodecMinimalTU_B.c CodecMinimalTU_Main.c -o $@
$(BUILD)/OfflineMultiTU: OfflineTU_A.c OfflineTU_B.c OfflineTU_Main.c $(HEADERS) | $(BUILD)
	$(CC) $(CFLAGS) OfflineTU_A.c OfflineTU_B.c OfflineTU_Main.c -o $@
$(BUILD)/MultiTU: MultiTU_A.c MultiTU_B.c MultiTU_Main.c $(HEADERS) | $(BUILD)
	$(CC) $(CFLAGS) MultiTU_A.c MultiTU_B.c MultiTU_Main.c -o $@
build: $(TESTS:%=$(BUILD)/%) $(BUILD)/ManualHardware $(BUILD)/CodecMinimalMultiTU $(BUILD)/OfflineMultiTU $(BUILD)/MultiTU
run: build
	@set -e; for test in $(TESTS:%=$(BUILD)/%) $(BUILD)/CodecMinimalMultiTU $(BUILD)/OfflineMultiTU $(BUILD)/MultiTU; do \
	  echo "[audio] run $$test"; \
	  if "$$test"; then :; else result=$$?; echo "[audio] FAILED $$test (exit $$result)"; exit "$$result"; fi; \
	done
hardware-test: $(BUILD)/ManualHardware
	@echo "WARNING: this opens real audio hardware and plays a short tone."
	$(BUILD)/ManualHardware
clean:
	rm -rf $(BUILD) roundtrip.wav
.PHONY: all build run clean hardware-test
