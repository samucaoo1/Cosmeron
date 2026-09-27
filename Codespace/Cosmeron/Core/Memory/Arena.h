#pragma once

#include "../Preprocessor/Compiling.inc"
#include "Memory.space"
#include "Alloc.h"
#include <string.h>

#define ARENA_TYPE(NAME) GNS2(LIB_PREFIX(Arena), NAME)
#define ARENA_FUNC(NAME) GNS2(LIB_PREFIX(Arena), NAME)
#define ARENA_CONST(NAME) CNS2(LIB_PREFIX_CONST(ARENA), NAME)

#define ARENA_ALIGNMENT MEMORY_MAX_ALIGNMENT

typedef struct ARENA_TYPE(TArena) {
  unsigned char *buffer;
  size_t capacity;
  size_t offset;
} TArena;

#define ARENA_CREATE_PROTOTYPE                                                 \
  static inline OPSTATUS ARENA_FUNC(Create)(TArena *arena, size_t capacity)

#define ARENA_ALLOC_PROTOTYPE                                                  \
  static inline OPSTATUS ARENA_FUNC(Alloc)(TArena *arena, void **out,           \
                                            size_t size)

#define ARENA_ALLOC_ALIGNED_PROTOTYPE                                          \
  static inline OPSTATUS ARENA_FUNC(AllocAligned)(TArena *arena, void **out,    \
                                                   size_t size, size_t alignment)

#define ARENA_ALLOC_ARRAY_PROTOTYPE                                            \
  static inline OPSTATUS ARENA_FUNC(AllocArray)(TArena *arena, void **out,     \
                                                 size_t count, size_t elementSize)

#define ARENA_ALLOC_ARRAY_ALIGNED_PROTOTYPE                                    \
  static inline OPSTATUS ARENA_FUNC(AllocArrayAligned)(                        \
      TArena *arena, void **out, size_t count, size_t elementSize,             \
      size_t alignment)

#define ARENA_ALLOC_TYPED_PROTOTYPE                                           \
  static inline OPSTATUS ARENA_FUNC(AllocTyped)(                               \
      TArena *arena, void *outPointerObject, size_t count,                     \
      size_t elementSize, size_t alignment)

#define ARENA_RESET_PROTOTYPE                                                  \
  static inline OPSTATUS ARENA_FUNC(Reset)(TArena *arena)

#define ARENA_DESTROY_PROTOTYPE                                                \
  static inline OPSTATUS ARENA_FUNC(Destroy)(TArena *arena)

ARENA_CREATE_PROTOTYPE;
ARENA_ALLOC_PROTOTYPE;
ARENA_ALLOC_ALIGNED_PROTOTYPE;
ARENA_ALLOC_ARRAY_PROTOTYPE;
ARENA_ALLOC_ARRAY_ALIGNED_PROTOTYPE;
ARENA_ALLOC_TYPED_PROTOTYPE;
ARENA_RESET_PROTOTYPE;
ARENA_DESTROY_PROTOTYPE;

#define Memory_Arena_Alloc(arena, type, out, count)                            \
  ARENA_FUNC(AllocTyped)((arena), (void *)(out), (count), sizeof(type),        \
                          _Alignof(type))

#include "Impl/Arena.impl"
/* EOF */
