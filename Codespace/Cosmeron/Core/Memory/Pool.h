#pragma once

#include "../Module/Object.h"
#include "../Preprocessor/Compiling.inc"
#include "Alloc.h"
#include "Memory.space"

#define POOL_NS(NAME) GNS2(MEMORY_NS(Pool), NAME)
#define POOL_CNS(NAME) CNS3(LIB_PREFIX_CONST(MEMORY_CMOD), POOL, NAME)

#define POOL_TYPE(NAME) POOL_NS(NAME)
#define POOL_FUNC(NAME) GNS2(POOL_TYPE(TMemoryPool), NAME)
#define POOL_CONST(NAME) POOL_CNS(NAME)

/* Forward declaration for typed node */
typedef struct MEMORY_INS(PoolNode) {
  struct MEMORY_INS(PoolNode) * next;
} MEMORY_INS(PoolNode);

/* Object-oriented Memory Pool structure definition with canonical namespace */
OBJECT(POOL_TYPE(TMemoryPool),
       FIELDS(void *buffer; size_t capacity; size_t elementSize;
              size_t elementCount; size_t allocatedCount; bool isOwned;
              MEMORY_INS(PoolNode) * freeList;),
       METHOD(OPSTATUS, Create, (size_t elementSize, size_t elementCount)),
       METHOD(OPSTATUS, CreateBuffer,
              (void *buffer, size_t bufferSize, size_t elementSize)),
       METHOD(OPSTATUS, Alloc, (void **out)),
       METHOD(OPSTATUS, Free, (void *ptr)), METHOD(OPSTATUS, Reset, ()),
       METHOD(OPSTATUS, Destroy, ()), CONST_METHOD(size_t, Capacity, ()),
       CONST_METHOD(size_t, Available, ()), CONST_METHOD(size_t, Allocated, ()),
       CONST_METHOD(size_t, ElementSize, ()),
       CONST_METHOD(bool, Contains, (const void *ptr)));

/* User-facing canonical type */
typedef POOL_TYPE(TMemoryPool) TMemoryPool;

#include "Impl/Pool.impl"

/* EOF */
