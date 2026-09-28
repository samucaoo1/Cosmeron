#pragma once

#include "../Preprocessor/Compiling.inc"
#include "Memory.space"
#include "Alloc.h"
#include <string.h>

#define ARENA_NS(NAME) GNS2(MEMORY_NS(Arena), NAME)
#define ARENA_CNS(NAME) CNS3(LIB_PREFIX_CONST(MEMORY_CMOD), ARENA, NAME)

#define ARENA_TYPE(NAME) ARENA_NS(NAME)
#define ARENA_FUNC(NAME) ARENA_NS(NAME)
#define ARENA_CONST(NAME) ARENA_CNS(NAME)

#define COSMERON_MACRO_INTERNAL_ARENA_ALIGNMENT                               \
  COSMERON_MACRO_INTERNAL_MEMORY_MAX_ALIGNMENT

typedef struct ARENA_TYPE(TArena) {
  unsigned char *buffer;
  size_t capacity;
  size_t offset;
} ARENA_TYPE(TArena);

#define ARENA_CREATE_PROTOTYPE                                                 \
  static inline OPSTATUS ARENA_FUNC(Create)(ARENA_TYPE(TArena) *arena, size_t capacity)

#define ARENA_ALLOC_PROTOTYPE                                                  \
  static inline OPSTATUS ARENA_FUNC(Alloc)(ARENA_TYPE(TArena) *arena, void **out,           \
                                            size_t size)

#define ARENA_ALLOC_ALIGNED_PROTOTYPE                                          \
  static inline OPSTATUS ARENA_FUNC(AllocAligned)(ARENA_TYPE(TArena) *arena, void **out,    \
                                                   size_t size, size_t alignment)

#define ARENA_ALLOC_ARRAY_PROTOTYPE                                            \
  static inline OPSTATUS ARENA_FUNC(AllocArray)(ARENA_TYPE(TArena) *arena, void **out,     \
                                                 size_t count, size_t elementSize)

#define ARENA_ALLOC_ARRAY_ALIGNED_PROTOTYPE                                    \
  static inline OPSTATUS ARENA_FUNC(AllocArrayAligned)(                        \
      ARENA_TYPE(TArena) *arena, void **out, size_t count, size_t elementSize,             \
      size_t alignment)

#define ARENA_ALLOC_TYPED_PROTOTYPE                                           \
  static inline OPSTATUS ARENA_FUNC(AllocTyped)(                               \
      ARENA_TYPE(TArena) *arena, void *outPointerObject, size_t count,                     \
      size_t elementSize, size_t alignment)

#define ARENA_RESET_PROTOTYPE                                                  \
  static inline OPSTATUS ARENA_FUNC(Reset)(ARENA_TYPE(TArena) *arena)

#define ARENA_DESTROY_PROTOTYPE                                                \
  static inline OPSTATUS ARENA_FUNC(Destroy)(ARENA_TYPE(TArena) *arena)

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
