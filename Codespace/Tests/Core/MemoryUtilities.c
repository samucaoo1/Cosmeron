#include "../../Cosmeron/Core/Memory/Alloc.h"
#include "../../Cosmeron/Core/Memory/Arena.h"
#include "../../Cosmeron/Core/Memory/Swap.h"

#include <stdint.h>

typedef struct MemoryUtilities_Aligned {
  long double value;
  void *pointer;
} MemoryUtilities_Aligned;

int main(void) {
  ARENA_TYPE(TArena) arena = {0};
  int *values = NULL;
  int left = 1;
  int right = 2;
  unsigned char *byte = NULL;
  MemoryUtilities_Aligned *aligned = NULL;
  size_t savedOffset;
  void *raw = NULL;
  void *rawArray = NULL;
  void *rawAligned = NULL;
  void *rawArrayAligned = NULL;
  int *typedDirect = NULL;
  unsigned char overlap[4] = {1U, 2U, 3U, 4U};

  if (ARENA_FUNC(Create)(&arena, 256) != STATUS_CONST(SUCCESS))
    return 1;
  if (ARENA_FUNC(AllocBytes)(&arena, &raw, sizeof(int)) != STATUS_CONST(SUCCESS) ||
      raw == NULL)
    return 19;
  if (ARENA_FUNC(AllocArray)(&arena, &rawArray, 2U, sizeof(int)) !=
          STATUS_CONST(SUCCESS) ||
      rawArray == NULL)
    return 20;
  if (ARENA_FUNC(AllocAligned)(&arena, &rawAligned, sizeof(int),
                               _Alignof(int)) != STATUS_CONST(SUCCESS) ||
      rawAligned == NULL)
    return 16;
  if (ARENA_FUNC(AllocArrayAligned)(&arena, &rawArrayAligned, 2U,
                                    sizeof(int), _Alignof(int)) !=
          STATUS_CONST(SUCCESS) ||
      rawArrayAligned == NULL)
    return 17;
  if (ARENA_FUNC(AllocTyped)(&arena, &typedDirect, 1U, sizeof(int),
                             _Alignof(int)) != STATUS_CONST(SUCCESS) ||
      typedDirect == NULL)
    return 18;

  if (Memory_Arena_Alloc(&arena, int, &values, 4) != STATUS_CONST(SUCCESS))
    return 2;
  if (!values)
    return 3;

  if (Memory_Arena_Alloc(&arena, unsigned char, &byte, 1) !=
      STATUS_CONST(SUCCESS))
    return 4;
  if (Memory_Arena_Alloc(&arena, MemoryUtilities_Aligned, &aligned, 1) !=
      STATUS_CONST(SUCCESS))
    return 5;
  if (((uintptr_t)aligned % _Alignof(MemoryUtilities_Aligned)) != 0)
    return 6;

  savedOffset = arena.offset;
  values = (int *)(uintptr_t)1;
  if (Memory_Arena_Alloc(&arena, int, &values, SIZE_MAX) !=
          STATUS_CONST(ARITHMETIC_OVERFLOW) ||
      values != (int *)(uintptr_t)1 || arena.offset != savedOffset)
    return 7;

  if (ARENA_FUNC(Reset)(&arena) != STATUS_CONST(SUCCESS))
    return 8;
  if (ARENA_FUNC(Destroy)(&arena) != STATUS_CONST(SUCCESS) || arena.buffer)
    return 9;

  if (SWAP_FUNC(Bytes)(&left, &right, sizeof(left)) != STATUS_CONST(SUCCESS))
    return 10;
  if (left != 2 || right != 1)
    return 11;
  if (SWAP_FUNC(Bytes)(&left, &left, sizeof(left)) != STATUS_CONST(SUCCESS) ||
      left != 2)
    return 12;
  if (SWAP_FUNC(Bytes)(NULL, &right, sizeof(right)) !=
      STATUS_CONST(INVALID_ARGUMENT))
    return 13;
  if (SWAP_FUNC(Bytes)(NULL, NULL, 0) != STATUS_CONST(SUCCESS))
    return 14;
  if (SWAP_FUNC(Bytes)(overlap, overlap + 1, 3U) !=
      STATUS_CONST(INVALID_ARGUMENT))
    return 21;

  Memory_Swap(int, left, right);
  return left == 1 && right == 2 ? 0 : 15;
}
