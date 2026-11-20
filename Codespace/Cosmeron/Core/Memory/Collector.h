#pragma once

#include "../Condition/Status.h"
#include "../Module/Closure/Closure.h"
#include "../Module/Module.h"
#include "../Preprocessor/Compiling.inc"
#include "Alloc.h"
#include "Allocator.h"
#include "Heap.h"
#include "Impl/Common.inc"

#include <setjmp.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <string.h>

/* ========================================================================== */
/* 1. Constants and Callables                                                 */
/* ========================================================================== */

#define MEMORY_COLLECTOR_CONFIG_COL_NAME(NAME, VALUE) NAME
#define MEMORY_COLLECTOR_CONFIG_COL_VALUE(NAME, VALUE) VALUE
#define MEMORY_COLLECTOR_CONFIG_GET(FIELD, ROW)                                \
  PP_OP_CAT2(MEMORY_COLLECTOR_CONFIG_COL_, FIELD) ROW

#define MEMORY_COLLECTOR_CONFIG_TABLE(X)                                       \
  X((DEFAULT_INITIAL_HEAP_PAGES, 4U))                                          \
  X((DEFAULT_INITIAL_THRESHOLD, 65536U))                                       \
  X((DEFAULT_GROWTH_PERCENT, 200U))                                            \
  X((MIN_PACING_PERCENT, 25U))                                                 \
  X((MAX_PACING_PERCENT, 400U))                                                \
  X((MIN_SURVIVAL_PERCENT, 10U))                                               \
  X((PERCENT_BASE, 100U))                                                      \
  X((MAX_ROOTS, 128U))                                                         \
  X((WORKLIST_CHUNK_CAPACITY, 512U))

CLOSURE_PROCEDURE(
    MODULE_INS(Memory, CollectorFinalizer),
    (void *ptr)
);

#define MEMORY_COLLECTOR_FINALIZER_CALLBACK                                    \
  TCLOSURE(MODULE_INS(Memory, CollectorFinalizer))

#define MEMORY_COLLECTOR_FINALIZER_BIND(FN, CTX)                               \
  NS(CLOSURE_FUNC(MODULE_INS(Memory, CollectorFinalizer)), Bind)((FN), (CTX))

#define MEMORY_COLLECTOR_FIELD_BIT(TYPE, FIELD)                                \
  (1ULL << (offsetof(TYPE, FIELD) / sizeof(void *)))

#define MEMORY_COLLECTOR_DESCRIPTOR_DEF(NAME, TYPE, ...)                       \
  static const MODULE_TYPE(Memory, Collector_Descriptor) NAME = {               \
      .size = sizeof(TYPE),                                                    \
      .bitmap = (0ULL | __VA_ARGS__)                                           \
  }

/* ========================================================================== */
/* 2. Declarative Module: Memory (Internal Collector Structs, Object & Facade)*/
/* ========================================================================== */

MODULE(DOMAIN(Core), Memory,
    ENUM_EX(CollectorConfig, MEMORY_COLLECTOR_CONFIG_TABLE, PLAIN),
    STRUCT(Collector_Descriptor,
        FIELDS(
            size_t size;
            uint64_t bitmap;
        )
    ),
    ISTRUCT(CollectorWorklistChunk,
        FIELDS(
            void *items[MODULE_CONST(Memory, WORKLIST_CHUNK_CAPACITY)];
            size_t count;
            struct MODULE_ITYPE(Memory, CollectorWorklistChunk) *next;
        )
    ),
    ISTRUCT(CollectorWorklist,
        FIELDS(
            MODULE_ITYPE(Memory, CollectorWorklistChunk) *head;
            MODULE_ITYPE(Memory, CollectorWorklistChunk) *freeChunks;
            size_t totalPushed;
            bool overflow;
        )
    ),
    ISTRUCT(CollectorFinalizerEntry,
        FIELDS(
            void *object;
            MEMORY_COLLECTOR_FINALIZER_CALLBACK callback;
            struct MODULE_ITYPE(Memory, CollectorFinalizerEntry) *next;
        )
    ),
    ISTRUCT(CollectorRootEntry,
        FIELDS(
            const void *pointerAddress;
            bool active;
        )
    ),
    OBJECT(Collector,
        FIELDS(
            MODULE_TYPE(Memory, Heap) heap;
            MODULE_ITYPE(Memory, CollectorWorklist) worklist;
            MODULE_ITYPE(Memory, CollectorFinalizerEntry) *finalizers;
            MODULE_ITYPE(Memory, CollectorFinalizerEntry) *pendingFinalizers;
            MODULE_ITYPE(Memory, CollectorRootEntry) roots[MODULE_CONST(Memory, MAX_ROOTS)];
            const void *stackBottom;
            size_t nextThreshold;
            size_t liveBytesAfterCollect;
            size_t totalAllocatedSinceCollect;
            uint32_t growthPercent;
            uint32_t pauseDepth;
            bool isInitialized;
        ),
        OPERATION(Create, (size_t initialHeapPages, const void *stackBottom)),
        OPERATION(Destroy, ()),
        VARIANT(Alloc,
            OPERATION((void *outPointerObject, size_t size, uint8_t policy, const void *descriptor)),
            OPERATION(Typed, (void *outPointerObject, const MODULE_TYPE(Memory, Collector_Descriptor) *descriptor)),
            OPERATION(Atomic, (void *outPointerObject, size_t size))
        ),
        OPERATION(SetFinalizer, (const void *ptr, MEMORY_COLLECTOR_FINALIZER_CALLBACK finalizer)),
        OPERATION(PinRoot, (const void *rootPointerAddress)),
        OPERATION(UnpinRoot, (const void *rootPointerAddress)),
        OPERATION(Collect, ()),
        PROCEDURE(Pause, ()),
        PROCEDURE(Resume, ()),
        CONST_PREDICATE_INLINE(IsPaused, (), { return self && self->pauseDepth > 0U; }),
        CONST_FUNCTION_INLINE(size_t, LiveBytes, (), { return self ? self->liveBytesAfterCollect : 0U; }),
        CONST_FUNCTION_INLINE(size_t, Threshold, (), { return self ? self->nextThreshold : 0U; }),
        CONST_PREDICATE(Contains, (const void *ptr)),
        FUNCTION(MODULE_TYPE(Memory, Allocator), AsAllocator, ())
    ),
    PACKAGE(GlobalCollector,
        OPERATION(Startup, (const void *stackBottom)),
        OPERATION(Shutdown, ()),
        FUNCTION(MODULE_TYPE(Memory, Collector) *, DefaultCollector, ()),
        VARIANT(Alloc,
            OPERATION((void *outPointerObject, size_t size)),
            OPERATION(Typed, (void *outPointerObject, const MODULE_TYPE(Memory, Collector_Descriptor) *descriptor)),
            OPERATION(Atomic, (void *outPointerObject, size_t size))
        ),
        OPERATION(SetFinalizer, (const void *ptr, MEMORY_COLLECTOR_FINALIZER_CALLBACK finalizer)),
        OPERATION(PinRoot, (const void *rootPointerAddress)),
        OPERATION(UnpinRoot, (const void *rootPointerAddress)),
        OPERATION(Collect, ()),
        PROCEDURE(Pause, ()),
        PROCEDURE(Resume, ()),
        FUNCTION(size_t, LiveBytes, ()),
        PREDICATE(Contains, (const void *ptr))
    )
);

#include "Impl/Collector.impl"

/* EOF */
