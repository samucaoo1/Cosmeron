#include "../../Cosmeron/Core/Memory/Allocator.h"
#include "../../Cosmeron/Core/Memory/Arena.h"
#include "../../Cosmeron/Core/Memory/Pool.h"
#include "../../Cosmeron/Core/Memory/Stream.h"

#include <stdint.h>
#include <stdio.h>

int main(void) {
  /* 1. Arena Allocator over stack buffer */
  uint8_t backing[256] = {0};
  MODULE_TYPE(Memory, Arena) arena = {0};
  (void)MODULE_FUNC(Memory, Arena, Init)(&arena);
  (void)MODULE_FUNC(Memory, Arena, CreateBuffer)(&arena, backing,
                                                 sizeof(backing));

  void *block = NULL;
  if (OBJECT_CALL(&arena, AllocAligned, &block, 64U, 16U) ==
      STATUS_CONST(SUCCESS)) {
    printf("Arena used %zu/%zu bytes\n", OBJECT_CALL_0(&arena, Used),
           OBJECT_CALL_0(&arena, Capacity));
  }
  (void)OBJECT_CALL_0(&arena, Reset);

  /* 2. Fixed-size Memory Pool */
  MODULE_TYPE(Memory, Pool) pool = {0};
  (void)MODULE_FUNC(Memory, Pool, Init)(&pool);
  (void)OBJECT_CALL(&pool, Create, sizeof(uint64_t), 4U);

  void *slot = NULL;
  if (OBJECT_CALL(&pool, Alloc, &slot) == STATUS_CONST(SUCCESS)) {
    *(uint64_t *)slot = 1337ULL;
    printf("Pool slot value=%llu, available=%zu\n",
           (unsigned long long)*(uint64_t *)slot,
           OBJECT_CALL_0(&pool, Available));
    (void)OBJECT_CALL(&pool, Free, slot);
  }
  (void)OBJECT_CALL_0(&pool, Destroy);

  /* 3. Binary Memory Stream */
  uint8_t wire[16] = {0};
  MODULE_TYPE(Memory, Stream) stream = {0};
  (void)MODULE_FUNC(Memory, Stream, Init)(&stream);
  (void)MODULE_FUNC(Memory, Stream, InitWrite)(&stream, wire, sizeof(wire));
  (void)OBJECT_CALL(&stream, WriteUInt32LE, 0x12345678U);
  (void)OBJECT_CALL(&stream, Seek, 0, MODULE_CONST(Memory, Stream, SEEK_BEGIN));

  uint32_t decoded = 0U;
  (void)OBJECT_CALL(&stream, ReadUInt32LE, &decoded);
  printf("Stream decoded=0x%08X\n", decoded);

  return 0;
}
