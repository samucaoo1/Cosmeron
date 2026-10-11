#pragma once
#include "Sort.space"
#include "../../Core/Memory/Alloc.h"

#define SORT_RADIX_PROTOTYPE(TYPE, SUFFIX)                                     \
  static inline OPSTATUS SORT_TYPED_FUNC(Radix, SUFFIX)(                       \
      TYPE *data, size_t count, SORT_TYPE(TOrder) order)
#define SORT_RADIX_BUFFER_PROTOTYPE(TYPE, SUFFIX)                              \
  static inline OPSTATUS SORT_TYPED_FUNC(RadixWithBuffer, SUFFIX)(             \
      TYPE *data, size_t count, SORT_TYPE(TOrder) order,                      \
      TYPE *buffer, size_t bufferCount)

#include "Impl/Integer.impl"

SORT_RADIX_IMPLEMENT(uint32_t, U32, uint32_t, 32u)
SORT_RADIX_IMPLEMENT(int32_t, I32, uint32_t, 32u)
SORT_RADIX_IMPLEMENT(uint64_t, U64, uint64_t, 64u)
SORT_RADIX_IMPLEMENT(int64_t, I64, uint64_t, 64u)
