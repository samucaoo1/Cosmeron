CC ?= gcc
CFLAGS := -std=c11 -Wall -Wextra -Wpedantic -Werror -D_POSIX_C_SOURCE=200809L
BUILD := build
WINDOWS_CC ?= x86_64-w64-mingw32-gcc
WINDOWS_CFLAGS := -std=c11 -Wall -Wextra -Wpedantic -Werror
WINDOWS_BUILD := build-windows
WINDOWS_TESTS := include-all smoke tcp udp poller resolve nonblocking poller-close udp-clear-peer ipv6 multi-tu lifecycle socket-recreate poller-empty connection-nonblocking poller-descriptor-limit poller-timeout connection-arguments datagram-arguments surface include-order

.PHONY: all strict windows windows-run clean

all: strict

strict: $(BUILD)/include-all $(BUILD)/smoke $(BUILD)/tcp $(BUILD)/udp $(BUILD)/poller $(BUILD)/resolve $(BUILD)/nonblocking $(BUILD)/poller-close $(BUILD)/udp-clear-peer $(BUILD)/ipv6 $(BUILD)/multi-tu $(BUILD)/lifecycle $(BUILD)/socket-recreate $(BUILD)/poller-empty $(BUILD)/connection-nonblocking $(BUILD)/poller-descriptor-limit $(BUILD)/poller-timeout $(BUILD)/connection-arguments $(BUILD)/datagram-arguments $(BUILD)/surface $(BUILD)/include-order
	$(BUILD)/include-all
	$(BUILD)/smoke
	$(BUILD)/tcp
	$(BUILD)/udp
	$(BUILD)/poller
	$(BUILD)/resolve
	$(BUILD)/nonblocking
	$(BUILD)/poller-close
	$(BUILD)/udp-clear-peer
	$(BUILD)/ipv6
	$(BUILD)/multi-tu
	$(BUILD)/lifecycle
	$(BUILD)/socket-recreate
	$(BUILD)/poller-empty
	$(BUILD)/connection-nonblocking
	$(BUILD)/poller-descriptor-limit
	$(BUILD)/poller-timeout
	$(BUILD)/connection-arguments
	$(BUILD)/datagram-arguments
	$(BUILD)/surface
	$(BUILD)/include-order

$(BUILD):
	mkdir -p $(BUILD)

$(BUILD)/include-all: IncludeAll.c | $(BUILD)
	$(CC) $(CFLAGS) IncludeAll.c -o $@

$(BUILD)/smoke: Smoke.c | $(BUILD)
	$(CC) $(CFLAGS) Smoke.c -o $@

$(BUILD)/tcp: TCP.c | $(BUILD)
	$(CC) $(CFLAGS) TCP.c -o $@

$(BUILD)/udp: UDP.c | $(BUILD)
	$(CC) $(CFLAGS) UDP.c -o $@

$(BUILD)/poller: Poller.c | $(BUILD)
	$(CC) $(CFLAGS) Poller.c -o $@

$(BUILD)/resolve: Resolve.c | $(BUILD)
	$(CC) $(CFLAGS) Resolve.c -o $@

$(BUILD)/nonblocking: NonBlocking.c | $(BUILD)
	$(CC) $(CFLAGS) NonBlocking.c -o $@

$(BUILD)/poller-close: PollerClose.c | $(BUILD)
	$(CC) $(CFLAGS) PollerClose.c -o $@

$(BUILD)/udp-clear-peer: UDPClearPeer.c | $(BUILD)
	$(CC) $(CFLAGS) UDPClearPeer.c -o $@

$(BUILD)/ipv6: IPv6.c | $(BUILD)
	$(CC) $(CFLAGS) IPv6.c -o $@

$(BUILD)/multi-tu: MultiTU_Main.c MultiTU_A.c MultiTU_B.c | $(BUILD)
	$(CC) $(CFLAGS) MultiTU_Main.c MultiTU_A.c MultiTU_B.c -o $@

$(BUILD)/lifecycle: Lifecycle.c | $(BUILD)
	$(CC) $(CFLAGS) Lifecycle.c -o $@

$(BUILD)/socket-recreate: SocketRecreate.c | $(BUILD)
	$(CC) $(CFLAGS) SocketRecreate.c -o $@

$(BUILD)/poller-empty: PollerEmpty.c | $(BUILD)
	$(CC) $(CFLAGS) PollerEmpty.c -o $@

$(BUILD)/connection-nonblocking: ConnectionNonBlocking.c | $(BUILD)
	$(CC) $(CFLAGS) ConnectionNonBlocking.c -o $@

$(BUILD)/poller-descriptor-limit: PollerDescriptorLimit.c | $(BUILD)
	$(CC) $(CFLAGS) PollerDescriptorLimit.c -o $@

$(BUILD)/poller-timeout: PollerTimeout.c | $(BUILD)
	$(CC) $(CFLAGS) PollerTimeout.c -o $@

$(BUILD)/connection-arguments: ConnectionArguments.c | $(BUILD)
	$(CC) $(CFLAGS) ConnectionArguments.c -o $@

$(BUILD)/datagram-arguments: DatagramArguments.c | $(BUILD)
	$(CC) $(CFLAGS) DatagramArguments.c -o $@

$(BUILD)/surface: Surface.c | $(BUILD)
	$(CC) $(CFLAGS) Surface.c -o $@

$(BUILD)/include-order: IncludeOrder.c | $(BUILD)
	$(CC) $(CFLAGS) IncludeOrder.c -o $@

clean:
	rm -rf $(BUILD) $(WINDOWS_BUILD)


WINDOWS_SOURCES := \
  include-all:IncludeAll.c \
  smoke:Smoke.c \
  tcp:TCP.c \
  udp:UDP.c \
  poller:Poller.c \
  resolve:Resolve.c \
  nonblocking:NonBlocking.c \
  poller-close:PollerClose.c \
  udp-clear-peer:UDPClearPeer.c \
  ipv6:IPv6.c \
  lifecycle:Lifecycle.c \
  socket-recreate:SocketRecreate.c \
  poller-empty:PollerEmpty.c \
  connection-nonblocking:ConnectionNonBlocking.c \
  poller-descriptor-limit:PollerDescriptorLimit.c \
  poller-timeout:PollerTimeout.c \
  connection-arguments:ConnectionArguments.c \
  datagram-arguments:DatagramArguments.c \
  surface:Surface.c \
  include-order:IncludeOrder.c

windows: $(addprefix $(WINDOWS_BUILD)/,$(addsuffix .exe,$(WINDOWS_TESTS)))

$(WINDOWS_BUILD):
	mkdir -p $(WINDOWS_BUILD)

$(WINDOWS_BUILD)/multi-tu.exe: MultiTU_Main.c MultiTU_A.c MultiTU_B.c | $(WINDOWS_BUILD)
	$(WINDOWS_CC) $(WINDOWS_CFLAGS) MultiTU_Main.c MultiTU_A.c MultiTU_B.c -o $@

define WINDOWS_TEST_RULE
$(WINDOWS_BUILD)/$(1).exe: $(2) | $(WINDOWS_BUILD)
	$$(WINDOWS_CC) $$(WINDOWS_CFLAGS) $(2) -o $$@ $$(WINDOWS_LDLIBS)
endef

$(foreach pair,$(WINDOWS_SOURCES),$(eval $(call WINDOWS_TEST_RULE,$(word 1,$(subst :, ,$(pair))),$(word 2,$(subst :, ,$(pair))))))

windows-run: windows
	@command -v wine >/dev/null 2>&1 || { echo "wine not found"; exit 1; }
	@set -e; for test in $(WINDOWS_TESTS); do echo "$(WINDOWS_BUILD)/$$test.exe"; wine "$(WINDOWS_BUILD)/$$test.exe"; done
