#pragma once

#include "../Module/Object.h"
#include "../Preprocessor/Compiling.inc"
#include "Alloc.h"
#include "Memory.space"

#define POOL_MOD GNS2(MEMORY_MOD, Pool)
#define POOL_CMOD PP_OP_CAT3(MEMORY_CMOD, _, POOL)

#define POOL_NS(NAME) GNS3(LIB_PREFIX(MEMORY_MOD), Pool, NAME)
#define POOL_CNS(NAME) CNS3(LIB_PREFIX_CONST(MEMORY_CMOD), POOL, NAME)
#define POOL_INS(NAME) GNS4(LIB_PREFIX(MEMORY_MOD), Pool, Internal, NAME)
#define POOL_CINS(NAME) CNS4(LIB_PREFIX_CONST(MEMORY_CMOD), POOL, INTERNAL, NAME)

#define POOL_TYPE(NAME) POOL_NS(NAME)
#define POOL_FUNC(NAME) GNS2(POOL_TYPE(TMemoryPool), NAME)
#define POOL_CONST(NAME) POOL_CNS(NAME)

/* Forward declaration for typed node */
typedef struct POOL_INS(Node) {
  struct POOL_INS(Node) *next;
} POOL_INS(Node);

/* Object-oriented Memory Pool structure definition with canonical namespace */
OBJECT(POOL_TYPE(TMemoryPool),
    FIELDS(
        void *buffer;
        size_t capacity;
        size_t elementSize;
        size_t elementCount;
        size_t allocatedCount;
        bool isOwned;
        POOL_INS(Node) *freeList;
    ),
    OPERATION(Create, (size_t elementSize, size_t elementCount)),
    OPERATION(CreateBuffer, (void *buffer, size_t bufferSize, size_t elementSize)),
    OPERATION(Alloc, (void *outPointerObject)),
    OPERATION(Free, (void *ptr)),
    OPERATION(FreePointer, (void *inoutPointerObject)),
    OPERATION(Reset, ()),
    OPERATION(Destroy, ()),
    CONST_FUNCTION_INLINE(size_t, Capacity, (), { return self ? self->elementCount : 0U; }),
    CONST_FUNCTION_INLINE(size_t, Available, (), { return self ? (self->elementCount - self->allocatedCount) : 0U; }),
    CONST_FUNCTION_INLINE(size_t, Allocated, (), { return self ? self->allocatedCount : 0U; }),
    CONST_FUNCTION_INLINE(size_t, ElementSize, (), { return self ? self->elementSize : 0U; }),
    CONST_PREDICATE(Contains, (const void *ptr))
);

typedef POOL_TYPE(TMemoryPool) TMemoryPool;

#include "Impl/Pool.impl"

/* EOF */
