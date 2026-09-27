#pragma once

#include "../Preprocessor/Compiling.inc"
#include "Memory.space"

#define ALLOC_FUNC(NAME) GNS2(LIB_PREFIX(Memory), NAME)

typedef union ALLOC_FUNC(MaxAlignment) {
  long double longDoubleValue;
  double doubleValue;
  long long integerValue;
  void *pointerValue;
} ALLOC_FUNC(MaxAlignment);

#define MEMORY_MAX_ALIGNMENT _Alignof(ALLOC_FUNC(MaxAlignment))

#define ALLOC_BYTES_PROTOTYPE                                                  \
  static inline OPSTATUS ALLOC_FUNC(AllocBytes)(void **out, size_t size)

#define ALLOC_ARRAY_PROTOTYPE                                                  \
  static inline OPSTATUS ALLOC_FUNC(AllocArray)(void **out, size_t count,      \
                                                 size_t elementSize)

#define REALLOC_BYTES_PROTOTYPE                                                \
  static inline OPSTATUS ALLOC_FUNC(ReallocBytes)(void **ptr, size_t size)

#define REALLOC_ARRAY_PROTOTYPE                                                \
  static inline OPSTATUS ALLOC_FUNC(ReallocArray)(void **ptr, size_t count,    \
                                                   size_t elementSize)

#define FREE_POINTER_PROTOTYPE                                                 \
  static inline OPSTATUS ALLOC_FUNC(FreePointer)(void **ptr)

ALLOC_BYTES_PROTOTYPE;
ALLOC_ARRAY_PROTOTYPE;
REALLOC_BYTES_PROTOTYPE;
REALLOC_ARRAY_PROTOTYPE;
FREE_POINTER_PROTOTYPE;

/*
 * Public free operation: the argument is the pointer lvalue itself, not its
 * address. This keeps the ergonomic contract explicit and clears the lvalue.
 *
 * Allocation/reallocation deliberately use the raw void* APIs above. Typed
 * callers allocate through a local void* temporary and publish the converted
 * pointer only after SUCCESS, avoiding invalid T** -> void** aliasing.
 */
#define Memory_Free(pointerLvalue)                                             \
  do {                                                                         \
    free((void *)(pointerLvalue));                                             \
    (pointerLvalue) = NULL;                                                    \
  } while (0)

#include "Impl/Alloc.impl"
/* EOF */
