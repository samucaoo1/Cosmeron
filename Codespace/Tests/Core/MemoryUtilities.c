#include "../../Cosmeron/Core/Memory/Alloc.h"
#include "../../Cosmeron/Core/Memory/Arena.h"
#include "../../Cosmeron/Core/Memory/Swap.h"

#include <stdint.h>

TEMPLATE_DECLARE(SWAP, int, int);

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

  if (ARENA_FUNC(Init)(&arena) != STATUS_CONST(SUCCESS))
    return 100;
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
  if (ARENA_FUNC(AllocArrayAligned)(&arena, &typedDirect, 1U, sizeof(int),
                                    _Alignof(int)) != STATUS_CONST(SUCCESS) ||
      typedDirect == NULL)
    return 18;

  if (ARENA_FUNC(AllocArray)(&arena, &values, 4, sizeof(*values)) != STATUS_CONST(SUCCESS))
    return 2;
  if (!values)
    return 3;

  if (ARENA_FUNC(AllocArray)(&arena, &byte, 1, sizeof(*byte)) !=
      STATUS_CONST(SUCCESS))
    return 4;
  if (ARENA_FUNC(AllocArrayAligned)(&arena, &aligned, 1, sizeof(*aligned), _Alignof(MemoryUtilities_Aligned)) !=
      STATUS_CONST(SUCCESS))
    return 5;
  if (((uintptr_t)aligned % _Alignof(MemoryUtilities_Aligned)) != 0)
    return 6;

  savedOffset = ARENA_FUNC(Used)(&arena);
  values = (int *)(uintptr_t)1;
  if (ARENA_FUNC(AllocArray)(&arena, &values, SIZE_MAX, sizeof(*values)) !=
          STATUS_CONST(ARITHMETIC_OVERFLOW) ||
      values != (int *)(uintptr_t)1 || ARENA_FUNC(Used)(&arena) != savedOffset)
    return 7;

  /* Save and restore */
  size_t mark = 0;
  if (ARENA_FUNC(Save)(&arena, &mark) != STATUS_CONST(SUCCESS))
    return 22;
  void *extra = NULL;
  if (ARENA_FUNC(AllocBytes)(&arena, &extra, 32) != STATUS_CONST(SUCCESS))
    return 23;
  if (ARENA_FUNC(Restore)(&arena, mark) != STATUS_CONST(SUCCESS))
    return 24;
  if (ARENA_FUNC(Used)(&arena) != mark)
    return 25;

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

  SWAP_FUNC(int_Values)(&left, &right);
  return left == 1 && right == 2 ? 0 : 15;
}
