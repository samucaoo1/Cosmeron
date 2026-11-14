#pragma once

#include "../Condition/Status.h"
#include "../Module/Module.h"
#include "../Preprocessor/Compiling.inc"
#include "Alloc.h"
#include "Impl/Common.inc"

/* ========================================================================== */
/* Declarative Module: Memory (Internal PoolNode Struct & TMemoryPool Object) */
/* ========================================================================== */

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
        CONST_PREDICATE(Contains, (const void *ptr))
    )
);

#include "Impl/Pool.impl"
#include "Allocator.h"

/* ========================================================================== */
/* Allocator Adapter: Memory_Pool_AsAllocator                                 */
/* ========================================================================== */

static inline OPSTATUS MODULE_FUNC(Memory, Pool, AllocatorAlloc)(
    void *context, size_t size, size_t alignment, void **outPtr) {
  (void)alignment;
  MODULE_TYPE(Memory, Pool) *pool = (MODULE_TYPE(Memory, Pool) *)context;
  if (!pool || size > pool->elementSize) {
    return STATUS_CONST(OUT_OF_RANGE);
  }
  return MODULE_FUNC(Memory, Pool, Alloc)(pool, outPtr);
}

static inline void MODULE_FUNC(Memory, Pool, AllocatorFree)(void *context,
                                                            void *ptr,
                                                            size_t size) {
  (void)size;
  MODULE_TYPE(Memory, Pool) *pool = (MODULE_TYPE(Memory, Pool) *)context;
  if (pool && ptr) {
    (void)MODULE_FUNC(Memory, Pool, Free)(pool, ptr);
  }
}

static inline MODULE_TYPE(Memory, Allocator)
MODULE_FUNC(Memory, Pool, AsAllocator)(MODULE_TYPE(Memory, Pool) *self) {
  static const MODULE_TYPE(Memory, AllocatorOps) ops = {
      .Alloc = MODULE_FUNC(Memory, Pool, AllocatorAlloc),
      .Realloc = NULL,
      .Free = MODULE_FUNC(Memory, Pool, AllocatorFree),
      .capabilities = CNS(Memory, SUPPORTS_FREE) | CNS(Memory, BULK_RESET),
  };
  MODULE_TYPE(Memory, Allocator) alloc = {
      .context = self,
      .ops = &ops,
  };
  return alloc;
}

/* EOF */
