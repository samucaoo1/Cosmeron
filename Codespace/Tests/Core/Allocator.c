#include "../../Cosmeron/Core/Memory/Allocator.h"

#include <assert.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>

typedef struct {
  uint8_t buffer[256];
  size_t offset;
  size_t allocCount;
  size_t freeCount;
} MockArena;

static OPSTATUS MockArenaAlloc(void *context, size_t size, size_t alignment,
                               void **outPtr) {
  MockArena *arena = (MockArena *)context;
  size_t alignedOffset = (arena->offset + (alignment - 1U)) & ~(alignment - 1U);
  if (alignedOffset + size > sizeof(arena->buffer)) {
    return STATUS_CONST(OUT_OF_MEMORY);
  }
  *outPtr = &arena->buffer[alignedOffset];
  arena->offset = alignedOffset + size;
  arena->allocCount++;
  return STATUS_CONST(SUCCESS);
}

static void MockArenaFree(void *context, void *ptr, size_t size) {
  (void)ptr;
  (void)size;
  MockArena *arena = (MockArena *)context;
  arena->freeCount++;
}

static const MODULE_TYPE(Memory, AllocatorOps) MOCK_ARENA_OPS = {
    .Alloc = MockArenaAlloc,
    .Realloc = NULL, /* exercises fallback Alloc + memcpy + Free */
    .Free = MockArenaFree,
    .capabilities = CNS(Memory, BULK_RESET),
};

int main(void) {
  /* 1. Default system allocator */
  MODULE_TYPE(Memory, Allocator) sys = MODULE_FUNC(Memory, Allocator, Default)();
  assert(MODULE_FUNC(Memory, Allocator, IsValid)(&sys));

  void *ptr = NULL;
  assert(MODULE_FUNC(Memory, Allocator, Alloc)(&sys, 32U, &ptr) ==
         STATUS_CONST(SUCCESS));
  assert(ptr != NULL);
  memset(ptr, 0xAB, 32U);

  void *reallocPtr = NULL;
  assert(MODULE_FUNC(Memory, Allocator, Realloc)(&sys, ptr, 32U, 64U,
                                                 &reallocPtr) ==
         STATUS_CONST(SUCCESS));
  assert(reallocPtr != NULL);
  assert(((uint8_t *)reallocPtr)[0] == 0xAB);
  assert(((uint8_t *)reallocPtr)[31] == 0xAB);
  MODULE_FUNC(Memory, Allocator, Free)(&sys, reallocPtr, 64U);

  /* 2. NULL allocator falls back to Default */
  void *nullAllocPtr = NULL;
  assert(MODULE_FUNC(Memory, Allocator, Calloc)(NULL, 4U, sizeof(int),
                                                &nullAllocPtr) ==
         STATUS_CONST(SUCCESS));
  assert(nullAllocPtr != NULL);
  for (size_t i = 0; i < 4U; ++i) {
    assert(((int *)nullAllocPtr)[i] == 0);
  }
  MODULE_FUNC(Memory, Allocator, Free)(NULL, nullAllocPtr, 4U * sizeof(int));

  /* 3. Overflow check in Calloc */
  void *overflowPtr = NULL;
  assert(MODULE_FUNC(Memory, Allocator, Calloc)(&sys, SIZE_MAX, 2U,
                                                &overflowPtr) ==
         STATUS_CONST(OUT_OF_RANGE));

  /* 4. Custom allocator with Realloc == NULL fallback */
  MockArena arena = {0};
  MODULE_TYPE(Memory, Allocator) arenaAlloc = {
      .context = &arena,
      .ops = &MOCK_ARENA_OPS,
  };
  void *a1 = NULL;
  assert(MODULE_FUNC(Memory, Allocator, Alloc)(&arenaAlloc, 16U, &a1) ==
         STATUS_CONST(SUCCESS));
  memset(a1, 0x55, 16U);

  void *a2 = NULL;
  assert(MODULE_FUNC(Memory, Allocator, Realloc)(&arenaAlloc, a1, 16U, 32U,
                                                 &a2) ==
         STATUS_CONST(SUCCESS));
  assert(((uint8_t *)a2)[0] == 0x55);
  assert(((uint8_t *)a2)[15] == 0x55);
  assert(arena.allocCount == 2U);
  assert(arena.freeCount == 1U);

  printf("Allocator tests passed!\n");
  return 0;
}
