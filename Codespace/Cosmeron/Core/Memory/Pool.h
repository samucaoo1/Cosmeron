#pragma once

#include "../Condition/Status.h"
#include "../Module/Module.h"
#include "../Preprocessor/Compiling.inc"
#include "Alloc.h"
#include "Allocator.h"
#include "Impl/Common.inc"

/* ========================================================================== */
/* Declarative Module: Memory (Internal PoolNode Struct & TMemoryPool Object) */
/* ========================================================================== */

#if defined(MEMORY_DISABLE_FUNCTION_TABLE) &&                                  \
    !defined(COSMERON_DISABLE_FT_Memory)
#define COSMERON_DISABLE_FT_Memory 1
#endif
#if defined(MEMORY_POOL_DISABLE_FUNCTION_TABLE) &&                             \
    !defined(COSMERON_DISABLE_FT_Memory_Pool)
#define COSMERON_DISABLE_FT_Memory_Pool 1
#endif

MODULE(DOMAIN(Core), Memory,
    ISTRUCT(PoolNode,
        FIELDS(
            struct MODULE_ITYPE(Memory, PoolNode) *next;
        )
    ),
    OBJECT(Pool,
        FIELDS(
            void *buffer;
            size_t capacity;
            size_t elementSize;
            size_t elementCount;
            size_t allocatedCount;
            bool isOwned;
            MODULE_ITYPE(Memory, PoolNode) *freeList;
        ),
        VARIANT(Create,
            OPERATION((size_t elementSize, size_t elementCount)),
            OPERATION(Buffer, (void *buffer, size_t bufferSize, size_t elementSize))
        ),
        OPERATION(Alloc, (void *outPointerObject)),
        VARIANT(Free,
            OPERATION((void *ptr)),
            OPERATION(Pointer, (void *inoutPointerObject))
        ),
        OPERATION(Reset, ()),
        OPERATION(Destroy, ()),
        CONST_FUNCTION_INLINE(size_t, Capacity, (), { return self ? self->elementCount : 0U; }),
        CONST_FUNCTION_INLINE(size_t, Available, (), { return self ? (self->elementCount - self->allocatedCount) : 0U; }),
        CONST_FUNCTION_INLINE(size_t, Allocated, (), { return self ? self->allocatedCount : 0U; }),
        CONST_FUNCTION_INLINE(size_t, ElementSize, (), { return self ? self->elementSize : 0U; }),
        CONST_PREDICATE(Contains, (const void *ptr)),
        FUNCTION(MODULE_TYPE(Memory, Allocator), AsAllocator, ())
    )
);

#include "Impl/Pool.impl"

/* EOF */
