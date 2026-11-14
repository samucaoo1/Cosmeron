#pragma once

#include "../Condition/Status.h"
#include "../Module/Module.h"
#include "../Preprocessor/Compiling.inc"
#include "Alloc.h"
#include "Impl/Common.inc"

/* ========================================================================== */
/* Object Declaration: Memory_Arena                                           */
/* ========================================================================== */

MODULE(DOMAIN(Core), Memory,
    OBJECT(Arena,
        FIELDS(
            uint8_t *buffer;
            size_t capacity;
            size_t offset;
            bool isOwned;
        ),
        VARIANT(Create,
            OPERATION((size_t capacity)),
            OPERATION(Buffer, (void *buffer, size_t capacity))
        ),
        VARIANT(Alloc,
            OPERATION(Bytes, (void *outPointerObject, size_t size)),
            OPERATION(Aligned, (void *outPointerObject, size_t size, size_t alignment)),
            OPERATION(Array, (void *outPointerObject, size_t count, size_t elementSize)),
            OPERATION(ArrayAligned, (void *outPointerObject, size_t count, size_t elementSize, size_t alignment))
        ),
        CONST_OPERATION(Save, (size_t *outMark)),
        OPERATION(Restore, (size_t mark)),
        OPERATION(Reset, ()),
        OPERATION(Destroy, ()),
        CONST_FUNCTION_INLINE(size_t, Capacity, (), { return self ? self->capacity : 0U; }),
        CONST_FUNCTION_INLINE(size_t, Used, (), { return self ? self->offset : 0U; }),
        CONST_FUNCTION_INLINE(size_t, Remaining, (), {
          return (self && self->offset <= self->capacity) ? (self->capacity - self->offset) : 0U;
        }),
        CONST_PREDICATE(Contains, (const void *ptr))
    )
);

#include "Impl/Arena.impl"
#include "Allocator.h"

/* ========================================================================== */
/* Allocator Adapter: Memory_Arena_AsAllocator                                */
/* ========================================================================== */

static inline OPSTATUS MODULE_FUNC(Memory, Arena, AllocatorAlloc)(
    void *context, size_t size, size_t alignment, void **outPtr) {
  return MODULE_FUNC(Memory, Arena, AllocAligned)(
      (MODULE_TYPE(Memory, Arena) *)context, outPtr, size, alignment);
}

static inline void MODULE_FUNC(Memory, Arena, AllocatorFree)(void *context,
                                                             void *ptr,
                                                             size_t size) {
  (void)context;
  (void)ptr;
  (void)size;
}

static inline MODULE_TYPE(Memory, Allocator)
MODULE_FUNC(Memory, Arena, AsAllocator)(MODULE_TYPE(Memory, Arena) *self) {
  static const MODULE_TYPE(Memory, AllocatorOps) ops = {
      .Alloc = MODULE_FUNC(Memory, Arena, AllocatorAlloc),
      .Realloc = NULL,
      .Free = MODULE_FUNC(Memory, Arena, AllocatorFree),
      .capabilities = CNS(Memory, BULK_RESET),
  };
  MODULE_TYPE(Memory, Allocator) alloc = {
      .context = self,
      .ops = &ops,
  };
  return alloc;
}

/* EOF */
