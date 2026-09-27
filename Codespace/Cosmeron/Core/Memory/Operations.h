#pragma once

#include "../Preprocessor/Compiling.inc"
#include "Memory.space"

#define MEMORY_COPY_PROTOTYPE                                                  \
  static inline OPSTATUS MEMORY_FUNC(Copy)(void *destination,                  \
                                            const void *source, size_t size)

#define MEMORY_MOVE_PROTOTYPE                                                  \
  static inline OPSTATUS MEMORY_FUNC(Move)(void *destination,                  \
                                            const void *source, size_t size)

#define MEMORY_SET_PROTOTYPE                                                   \
  static inline OPSTATUS MEMORY_FUNC(Set)(void *destination, int value,        \
                                           size_t size)

#define MEMORY_ZERO_PROTOTYPE                                                  \
  static inline OPSTATUS MEMORY_FUNC(Zero)(void *destination, size_t size)

#define MEMORY_COPY_ARRAY_PROTOTYPE                                            \
  static inline OPSTATUS MEMORY_FUNC(CopyArray)(                               \
      void *destination, const void *source, size_t count, size_t elementSize)

#define MEMORY_MOVE_ARRAY_PROTOTYPE                                            \
  static inline OPSTATUS MEMORY_FUNC(MoveArray)(                               \
      void *destination, const void *source, size_t count, size_t elementSize)

#define MEMORY_ZERO_ARRAY_PROTOTYPE                                            \
  static inline OPSTATUS MEMORY_FUNC(ZeroArray)(                               \
      void *destination, size_t count, size_t elementSize)

MEMORY_COPY_PROTOTYPE;
MEMORY_MOVE_PROTOTYPE;
MEMORY_SET_PROTOTYPE;
MEMORY_ZERO_PROTOTYPE;
MEMORY_COPY_ARRAY_PROTOTYPE;
MEMORY_MOVE_ARRAY_PROTOTYPE;
MEMORY_ZERO_ARRAY_PROTOTYPE;

#define Memory_CopyTyped(type, destination, source, count)                          \
  MEMORY_FUNC(CopyArray)((destination), (source), (count), sizeof(type))

#define Memory_MoveTyped(type, destination, source, count)                          \
  MEMORY_FUNC(MoveArray)((destination), (source), (count), sizeof(type))

#define Memory_ZeroTyped(type, destination, count)                                  \
  MEMORY_FUNC(ZeroArray)((destination), (count), sizeof(type))

#include "Impl/Operations.impl"
/* EOF */
