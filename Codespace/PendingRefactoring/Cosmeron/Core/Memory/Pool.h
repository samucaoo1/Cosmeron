#pragma once

#include "../Module/Object.h"
#include "../Preprocessor/Compiling.inc"
#include "Alloc.h"
#include "Common.h"

#define POOL_NS(NAME) NS(LIB_PREFIX(Memory_Pool), NAME)
#define POOL_CNS(NAME) CNS(LIB_PREFIX_CONST(MEMORY), POOL, NAME)
#define POOL_INS(NAME) NS(LIB_PREFIX(Memory_Pool), Internal, NAME)
#define POOL_CINS(NAME) CNS(LIB_PREFIX_CONST(MEMORY), POOL, INTERNAL, NAME)

#define POOL_TYPE(NAME) MEMORY_TYPE(NAME)
#define POOL_FUNC(NAME) NS(MEMORY_TYPE(Pool), NAME)
#define POOL_CONST(NAME) POOL_CNS(NAME)

/* Forward declaration for typed node */
typedef struct POOL_INS(Node) {
  struct POOL_INS(Node) *next;
} POOL_INS(Node);

/* Object-oriented Memory Pool structure definition with canonical namespace */
OBJECT(MEMORY_TYPE(Pool),
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

#include "Impl/Pool.impl"

/* EOF */
