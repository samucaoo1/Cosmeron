CC ?= gcc
CFLAGS := -std=c11 -Wall -Wextra -Wpedantic -Werror -D_POSIX_C_SOURCE=200809L
BUILD := build
WINDOWS_CC ?= x86_64-w64-mingw32-gcc
WINDOWS_BUILD := build-windows
TESTS := include-all module-surface atomic mutex once rwlock semaphore thread coroutine condition barrier contention producer-consumer thread-pool thread-pool-multi-producer thread-pool-shutdown future

.PHONY: all strict gcc clang run zero-link windows clean
all: strict
strict: gcc
gcc:
	$(MAKE) CC=gcc BUILD=build run

clang:
	$(MAKE) CC=clang BUILD=build-clang run

run: $(addprefix $(BUILD)/,$(TESTS))
	@set -e; $(foreach test,$(TESTS),$(BUILD)/$(test);)

zero-link: $(BUILD)/thread-zero-link

$(BUILD):
	mkdir -p $(BUILD)
$(BUILD)/include-all: IncludeAll.c | $(BUILD)
	$(CC) $(CFLAGS) $< -o $@
$(BUILD)/module-surface: ModuleSurface.c | $(BUILD)
	$(CC) $(CFLAGS) $< -o $@
$(BUILD)/atomic: Atomic.c | $(BUILD)
	$(CC) $(CFLAGS) $< -o $@
$(BUILD)/mutex: Mutex.c | $(BUILD)
	$(CC) $(CFLAGS) $< -o $@
$(BUILD)/once: Once.c | $(BUILD)
	$(CC) $(CFLAGS) $< -o $@
$(BUILD)/rwlock: RWLock.c | $(BUILD)
	$(CC) $(CFLAGS) $< -o $@
$(BUILD)/semaphore: Semaphore.c | $(BUILD)
	$(CC) $(CFLAGS) $< -o $@
$(BUILD)/thread: Thread.c | $(BUILD)
	$(CC) $(CFLAGS) $< -o $@
$(BUILD)/coroutine: Coroutine.c | $(BUILD)
	$(CC) $(CFLAGS) $< -o $@
$(BUILD)/condition: Condition.c | $(BUILD)
	$(CC) $(CFLAGS) $< -o $@
$(BUILD)/barrier: Barrier.c | $(BUILD)
	$(CC) $(CFLAGS) $< -o $@
$(BUILD)/contention: Contention.c | $(BUILD)
	$(CC) $(CFLAGS) $< -o $@
$(BUILD)/producer-consumer: ProducerConsumer.c | $(BUILD)
	$(CC) $(CFLAGS) $< -o $@
$(BUILD)/thread-pool: ThreadPool.c | $(BUILD)
	$(CC) $(CFLAGS) $< -o $@
$(BUILD)/thread-pool-multi-producer: ThreadPoolMultiProducer.c | $(BUILD)
	$(CC) $(CFLAGS) $< -o $@
$(BUILD)/thread-pool-shutdown: ThreadPoolShutdown.c | $(BUILD)
	$(CC) $(CFLAGS) $< -o $@
$(BUILD)/future: Future.c | $(BUILD)
	$(CC) $(CFLAGS) $< -o $@

# Deliberately no -pthread/-latomic here. Thread is a separate zero-link probe.
$(BUILD)/thread-zero-link: Thread.c | $(BUILD)
	$(CC) $(CFLAGS) $< -o $@

windows: | $(WINDOWS_BUILD)
	$(WINDOWS_CC) $(CFLAGS) IncludeAll.c -o $(WINDOWS_BUILD)/include-all.exe
	$(WINDOWS_CC) $(CFLAGS) ModuleSurface.c -o $(WINDOWS_BUILD)/module-surface.exe
	$(WINDOWS_CC) $(CFLAGS) Atomic.c -o $(WINDOWS_BUILD)/atomic.exe
	$(WINDOWS_CC) $(CFLAGS) Mutex.c -o $(WINDOWS_BUILD)/mutex.exe
	$(WINDOWS_CC) $(CFLAGS) Once.c -o $(WINDOWS_BUILD)/once.exe
	$(WINDOWS_CC) $(CFLAGS) Thread.c -o $(WINDOWS_BUILD)/thread.exe
	$(WINDOWS_CC) $(CFLAGS) Coroutine.c -o $(WINDOWS_BUILD)/coroutine.exe
	$(WINDOWS_CC) $(CFLAGS) RWLock.c -o $(WINDOWS_BUILD)/rwlock.exe
	$(WINDOWS_CC) $(CFLAGS) Semaphore.c -o $(WINDOWS_BUILD)/semaphore.exe
	$(WINDOWS_CC) $(CFLAGS) Condition.c -o $(WINDOWS_BUILD)/condition.exe
	$(WINDOWS_CC) $(CFLAGS) Barrier.c -o $(WINDOWS_BUILD)/barrier.exe
	$(WINDOWS_CC) $(CFLAGS) Contention.c -o $(WINDOWS_BUILD)/contention.exe
	$(WINDOWS_CC) $(CFLAGS) ProducerConsumer.c -o $(WINDOWS_BUILD)/producer-consumer.exe
	$(WINDOWS_CC) $(CFLAGS) ThreadPool.c -o $(WINDOWS_BUILD)/thread-pool.exe
	$(WINDOWS_CC) $(CFLAGS) ThreadPoolMultiProducer.c -o $(WINDOWS_BUILD)/thread-pool-multi-producer.exe
	$(WINDOWS_CC) $(CFLAGS) ThreadPoolShutdown.c -o $(WINDOWS_BUILD)/thread-pool-shutdown.exe
	$(WINDOWS_CC) $(CFLAGS) Future.c -o $(WINDOWS_BUILD)/future.exe
$(WINDOWS_BUILD):
	mkdir -p $(WINDOWS_BUILD)
clean:
	rm -rf build build-clang $(WINDOWS_BUILD)
