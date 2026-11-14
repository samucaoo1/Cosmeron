#pragma once

#include <stddef.h>
#include <stdint.h>
#include <string.h>

#include "../Module/Module.h"
#include "../Condition/Status.h"
#include "Alloc.h"

/* ========================================================================== */
/* Allocator Capabilities X-Macro Table                                       */
/* ========================================================================== */

#define MEMORY_ALLOCATOR_CAPABILITY_TABLE(X)                                   \
  X((NONE, 0U))                                                                \
  X((SUPPORTS_REALLOC, 1U << 0))                                               \
  X((SUPPORTS_FREE, 1U << 1))                                                  \
  X((THREAD_SAFE, 1U << 2))                                                    \
  X((BULK_RESET, 1U << 3))

/* ========================================================================== */
/* Allocator Interface Types & Package                                        */
/* ========================================================================== */

MODULE(DOMAIN(Core), Memory,
    ENUM(AllocatorCapability, MEMORY_ALLOCATOR_CAPABILITY_TABLE),
    STRUCT(AllocatorOps,
        FIELDS(
            OPSTATUS (*Alloc)(void *context, size_t size, size_t alignment,
                              void **outPtr);
            OPSTATUS (*Realloc)(void *context, void *ptr, size_t oldSize,
                                size_t newSize, size_t alignment,
                                void **outPtr);
            void (*Free)(void *context, void *ptr, size_t size);
            uint32_t capabilities;
        )
    ),
    STRUCT(Allocator,
        FIELDS(
            void *context;
            const MODULE_TYPE(Memory, AllocatorOps) *ops;
        )
    )
);

/* ========================================================================== */
/* Default System Allocator Adapter                                           */
/* ========================================================================== */

static inline OPSTATUS MODULE_FUNC(Memory, Allocator, SystemAlloc)(
    void *context, size_t size, size_t alignment, void **outPtr) {
  (void)context;
  (void)alignment;
  if (!outPtr) {
    return STATUS_CONST(INVALID_ARGUMENT);
  }
  return MODULE_FUNC(Memory, Alloc, Bytes)(outPtr, size);
}

static inline OPSTATUS MODULE_FUNC(Memory, Allocator, SystemRealloc)(
    void *context, void *ptr, size_t oldSize, size_t newSize, size_t alignment,
    void **outPtr) {
  (void)context;
  (void)oldSize;
  (void)alignment;
  if (!outPtr) {
    return STATUS_CONST(INVALID_ARGUMENT);
  }
  void *nextPtr = ptr;
  OPSTATUS status = MODULE_FUNC(Memory, Alloc, ReallocBytes)(&nextPtr, newSize);
  if (status == STATUS_CONST(SUCCESS)) {
    *outPtr = nextPtr;
  }
  return status;
}

static inline void MODULE_FUNC(Memory, Allocator, SystemFree)(void *context,
                                                              void *ptr,
                                                              size_t size) {
  (void)context;
  (void)size;
  if (ptr) {
    void *target = ptr;
    (void)MODULE_FUNC(Memory, Alloc, Free)(&target);
  }
}

static inline const MODULE_TYPE(Memory, AllocatorOps) *
MODULE_FUNC(Memory, Allocator, SystemOps)(void) {
  static const MODULE_TYPE(Memory, AllocatorOps) ops = {
      .Alloc = MODULE_FUNC(Memory, Allocator, SystemAlloc),
      .Realloc = MODULE_FUNC(Memory, Allocator, SystemRealloc),
      .Free = MODULE_FUNC(Memory, Allocator, SystemFree),
      .capabilities =
          CNS(Memory, SUPPORTS_REALLOC) | CNS(Memory, SUPPORTS_FREE),
  };
  return &ops;
}

/* ========================================================================== */
/* Package Declaration: Memory_Allocator                                      */
/* ========================================================================== */

MODULE(DOMAIN(Core), Memory,
    PACKAGE(Allocator,
        FUNCTION(MODULE_TYPE(Memory, Allocator), Default, (void)),
        PREDICATE(IsValid, (const MODULE_TYPE(Memory, Allocator) *self)),
        OPERATION(AllocAligned,
            (const MODULE_TYPE(Memory, Allocator) *self, size_t size,
             size_t alignment, void **outPtr)),
        OPERATION(Alloc,
            (const MODULE_TYPE(Memory, Allocator) *self, size_t size,
             void **outPtr)),
        OPERATION(Calloc,
            (const MODULE_TYPE(Memory, Allocator) *self, size_t count,
             size_t elementSize, void **outPtr)),
        OPERATION(Realloc,
            (const MODULE_TYPE(Memory, Allocator) *self, void *ptr,
             size_t oldSize, size_t newSize, void **outPtr)),
        PROCEDURE(Free,
            (const MODULE_TYPE(Memory, Allocator) *self, void *ptr,
             size_t size))
    )
);

/* ========================================================================== */
/* Package Implementation: Memory_Allocator                                   */
/* ========================================================================== */

static inline MODULE_TYPE(Memory, Allocator)
MODULE_FUNC(Memory, Allocator, Default)(void) {
  MODULE_TYPE(Memory, Allocator) alloc = {
      .context = NULL,
      .ops = MODULE_FUNC(Memory, Allocator, SystemOps)(),
  };
  return alloc;
}

static inline bool MODULE_FUNC(Memory, Allocator, IsValid)(
    const MODULE_TYPE(Memory, Allocator) *self) {
  return self != NULL && self->ops != NULL && self->ops->Alloc != NULL;
}

static inline OPSTATUS MODULE_FUNC(Memory, Allocator, AllocAligned)(
    const MODULE_TYPE(Memory, Allocator) *self, size_t size, size_t alignment,
    void **outPtr) {
  if (!outPtr || size == 0U || alignment == 0U ||
      (alignment & (alignment - 1U)) != 0U) {
    return STATUS_CONST(INVALID_ARGUMENT);
  }
  MODULE_TYPE(Memory, Allocator) fallback;
  const MODULE_TYPE(Memory, Allocator) *resolved = self;
  if (!resolved) {
    fallback = MODULE_FUNC(Memory, Allocator, Default)();
    resolved = &fallback;
  }
  if (!resolved->ops || !resolved->ops->Alloc) {
    return STATUS_CONST(INVALID_ARGUMENT);
  }
  return resolved->ops->Alloc(resolved->context, size, alignment, outPtr);
}

static inline OPSTATUS MODULE_FUNC(Memory, Allocator, Alloc)(
    const MODULE_TYPE(Memory, Allocator) *self, size_t size, void **outPtr) {
  return MODULE_FUNC(Memory, Allocator, AllocAligned)(
      self, size, _Alignof(max_align_t), outPtr);
}

static inline OPSTATUS MODULE_FUNC(Memory, Allocator, Calloc)(
    const MODULE_TYPE(Memory, Allocator) *self, size_t count,
    size_t elementSize, void **outPtr) {
  if (!outPtr || count == 0U || elementSize == 0U) {
    return STATUS_CONST(INVALID_ARGUMENT);
  }
  if (count > SIZE_MAX / elementSize) {
    return STATUS_CONST(OUT_OF_RANGE);
  }
  size_t totalSize = count * elementSize;
  void *ptr = NULL;
  OPSTATUS status = MODULE_FUNC(Memory, Allocator, AllocAligned)(
      self, totalSize, _Alignof(max_align_t), &ptr);
  if (status != STATUS_CONST(SUCCESS)) {
    return status;
  }
  memset(ptr, 0, totalSize);
  *outPtr = ptr;
  return STATUS_CONST(SUCCESS);
}

static inline OPSTATUS MODULE_FUNC(Memory, Allocator, Realloc)(
    const MODULE_TYPE(Memory, Allocator) *self, void *ptr, size_t oldSize,
    size_t newSize, void **outPtr) {
  if (!outPtr || newSize == 0U) {
    return STATUS_CONST(INVALID_ARGUMENT);
  }
  MODULE_TYPE(Memory, Allocator) fallback;
  const MODULE_TYPE(Memory, Allocator) *resolved = self;
  if (!resolved) {
    fallback = MODULE_FUNC(Memory, Allocator, Default)();
    resolved = &fallback;
  }
  if (!resolved->ops || !resolved->ops->Alloc) {
    return STATUS_CONST(INVALID_ARGUMENT);
  }
  if (resolved->ops->Realloc) {
    return resolved->ops->Realloc(resolved->context, ptr, oldSize, newSize,
                                  _Alignof(max_align_t), outPtr);
  }
  void *nextPtr = NULL;
  OPSTATUS status = resolved->ops->Alloc(resolved->context, newSize,
                                         _Alignof(max_align_t), &nextPtr);
  if (status != STATUS_CONST(SUCCESS)) {
    return status;
  }
  if (ptr && oldSize > 0U) {
    size_t copySize = oldSize < newSize ? oldSize : newSize;
    memcpy(nextPtr, ptr, copySize);
    if (resolved->ops->Free) {
      resolved->ops->Free(resolved->context, ptr, oldSize);
    }
  }
  *outPtr = nextPtr;
  return STATUS_CONST(SUCCESS);
}

static inline void MODULE_FUNC(Memory, Allocator, Free)(
    const MODULE_TYPE(Memory, Allocator) *self, void *ptr, size_t size) {
  if (!ptr) {
    return;
  }
  MODULE_TYPE(Memory, Allocator) fallback;
  const MODULE_TYPE(Memory, Allocator) *resolved = self;
  if (!resolved) {
    fallback = MODULE_FUNC(Memory, Allocator, Default)();
    resolved = &fallback;
  }
  if (resolved->ops && resolved->ops->Free) {
    resolved->ops->Free(resolved->context, ptr, size);
  }
}

/* EOF */
