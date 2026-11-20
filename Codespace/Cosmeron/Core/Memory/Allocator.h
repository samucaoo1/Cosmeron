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

#define MEMORY_ALLOCATOR_CAPABILITY_COL_NAME(NAME, VALUE) NAME
#define MEMORY_ALLOCATOR_CAPABILITY_COL_VALUE(NAME, VALUE) VALUE
#define MEMORY_ALLOCATOR_CAPABILITY_GET(FIELD, ROW)                            \
  PP_OP_CAT2(MEMORY_ALLOCATOR_CAPABILITY_COL_, FIELD) ROW

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
    STRUCT(Allocator_Ops,
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
            const MODULE_TYPE(Memory, Allocator_Ops) *ops;
        )
    ),
    PACKAGE(Allocator,
        FUNCTION(MODULE_TYPE(Memory, Allocator), Default, (void)),
        PREDICATE(IsValid, (const MODULE_TYPE(Memory, Allocator) *self)),
        VARIANT(Alloc,
            OPERATION(
                (const MODULE_TYPE(Memory, Allocator) *self, size_t size,
                 void **outPtr)),
            OPERATION(Aligned,
                (const MODULE_TYPE(Memory, Allocator) *self, size_t size,
                 size_t alignment, void **outPtr))
        ),
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

#include "Impl/Allocator.impl"

/* EOF */
