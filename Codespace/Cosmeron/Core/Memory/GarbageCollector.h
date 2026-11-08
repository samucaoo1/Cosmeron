#pragma once

#include "../Outcome/Status.h"
#include "../Module/Callback/Callback.h"
#include "../Module/Module.h"
#include "../Preprocessor/Compiling.inc"
#include "../Preprocessor/Detect/OperationSystem.h"
#include "Alloc.h"
#include "Heap.h"
#include "Common.inc"

#include <setjmp.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <string.h>

/* ========================================================================== */
/* 1. Constants and Callbacks                                                 */
/* ========================================================================== */

#define GARBAGE_COLLECTOR_CONFIG_COL_NAME(NAME, VALUE) NAME
#define GARBAGE_COLLECTOR_CONFIG_COL_VALUE(NAME, VALUE) VALUE
#define GARBAGE_COLLECTOR_CONFIG_GET(FIELD, ROW) PP_OP_CAT2(GARBAGE_COLLECTOR_CONFIG_COL_, FIELD) ROW

#define GARBAGE_COLLECTOR_CONFIG_TABLE(X) \
  X((DEFAULT_INITIAL_THRESHOLD, 65536U))  \
  X((DEFAULT_GROWTH_PERCENT,    200U))    \
  X((MIN_PACING_PERCENT,        25U))     \
  X((MAX_PACING_PERCENT,        400U))    \
  X((MAX_ROOTS,                 128U))    \
  X((WORKLIST_CHUNK_CAPACITY,   512U))

enum MODULE_ENUM(Memory, GarbageCollector, Config) {
#define GARBAGE_COLLECTOR_CONFIG_ENUM_ITEM(ROW) \
  MODULE_CONST(MEMORY, GARBAGE_COLLECTOR, GARBAGE_COLLECTOR_CONFIG_GET(NAME, ROW)) = \
      GARBAGE_COLLECTOR_CONFIG_GET(VALUE, ROW),
  GARBAGE_COLLECTOR_CONFIG_TABLE(GARBAGE_COLLECTOR_CONFIG_ENUM_ITEM)
#undef GARBAGE_COLLECTOR_CONFIG_ENUM_ITEM
};

CALLBACK_PROCEDURE(
    MODULE_INS(Memory, GcFinalizerFunction),
    (void *ptr)
);

#define GARBAGE_COLLECTOR_FINALIZER_CALLBACK \
  TCALLBACK(MODULE_INS(Memory, GcFinalizerFunction))

#define GARBAGE_COLLECTOR_FINALIZER_BIND(FN, CTX) \
  NS(CALLBACK_FUNC(MODULE_INS(Memory, GcFinalizerFunction)), Bind)((FN), (CTX))

/* Typed struct descriptor */
typedef struct MODULE_NS(Memory, GarbageCollector, Descriptor) {
  size_t size;
  uint64_t bitmap;
} MODULE_NS(Memory, GarbageCollector, Descriptor);

#define GARBAGE_COLLECTOR_FIELD_BIT(TYPE, FIELD) \
  (1ULL << (offsetof(TYPE, FIELD) / sizeof(void *)))

#define GARBAGE_COLLECTOR_DESCRIPTOR_DEF(NAME, TYPE, ...) \
  static const MODULE_NS(Memory, GarbageCollector, Descriptor) NAME = { \
      .size = sizeof(TYPE), \
      .bitmap = (0ULL | __VA_ARGS__) \
  }

/* ========================================================================== */
/* 2. Internal Collector Data Structures                                      */
/* ========================================================================== */

typedef struct MODULE_ITYPE(Memory, GcWorklistChunk) {
  void *items[MODULE_CONST(MEMORY, GARBAGE_COLLECTOR, WORKLIST_CHUNK_CAPACITY)];
  size_t count;
  struct MODULE_ITYPE(Memory, GcWorklistChunk) *next;
} MODULE_ITYPE(Memory, GcWorklistChunk);

typedef struct MODULE_ITYPE(Memory, GcWorklist) {
  MODULE_ITYPE(Memory, GcWorklistChunk) *head;
  MODULE_ITYPE(Memory, GcWorklistChunk) *freeChunks;
  size_t totalPushed;
  bool overflow;
} MODULE_ITYPE(Memory, GcWorklist);

typedef struct MODULE_ITYPE(Memory, GcFinalizerEntry) {
  void *object;
  GARBAGE_COLLECTOR_FINALIZER_CALLBACK callback;
  struct MODULE_ITYPE(Memory, GcFinalizerEntry) *next;
} MODULE_ITYPE(Memory, GcFinalizerEntry);

typedef struct MODULE_ITYPE(Memory, GcRootEntry) {
  const void *pointerAddress;
  bool active;
} MODULE_ITYPE(Memory, GcRootEntry);

/* ========================================================================== */
/* 3. Declarative TMemoryGarbageCollector Object & GC Package Facade          */
/* ========================================================================== */

MODULE(DOMAIN(Core), Memory,
    OBJECT(GarbageCollector,
        FIELDS(
            MODULE_TYPE(Memory, Heap) heap;
            MODULE_ITYPE(Memory, GcWorklist) worklist;
            MODULE_ITYPE(Memory, GcFinalizerEntry) *finalizers;
            MODULE_ITYPE(Memory, GcFinalizerEntry) *pendingFinalizers;
            MODULE_ITYPE(Memory, GcRootEntry) roots[MODULE_CONST(MEMORY, GARBAGE_COLLECTOR, MAX_ROOTS)];
            const void *stackBottom;
            size_t nextThreshold;
            size_t liveBytesAfterGC;
            size_t totalAllocatedSinceGC;
            uint32_t growthPercent;
            uint32_t pauseDepth;
            bool isInitialized;
        ),
        OPERATION(Create, (size_t initialHeapPages, const void *stackBottom)),
        OPERATION(Destroy, ()),
        OPERATION(Alloc, (void *outPointerObject, size_t size, uint8_t policy, const void *descriptor)),
        OPERATION(AllocTyped, (void *outPointerObject, const MODULE_NS(Memory, GarbageCollector, Descriptor) *descriptor)),
        OPERATION(AllocAtomic, (void *outPointerObject, size_t size)),
        OPERATION(SetFinalizer, (const void *ptr, GARBAGE_COLLECTOR_FINALIZER_CALLBACK finalizer)),
        OPERATION(PinRoot, (const void *rootPointerAddress)),
        OPERATION(UnpinRoot, (const void *rootPointerAddress)),
        OPERATION(Collect, ()),
        PROCEDURE(Pause, ()),
        PROCEDURE(Resume, ()),
        CONST_PREDICATE_INLINE(IsPaused, (), { return self && self->pauseDepth > 0U; }),
        CONST_FUNCTION_INLINE(size_t, LiveBytes, (), { return self ? self->liveBytesAfterGC : 0U; }),
        CONST_FUNCTION_INLINE(size_t, Threshold, (), { return self ? self->nextThreshold : 0U; }),
        CONST_PREDICATE(Contains, (const void *ptr))
    ),
    PACKAGE(GC,
        OPERATION(Startup, (const void *stackBottom)),
        OPERATION(Shutdown, ()),
        FUNCTION(MODULE_TYPE(Memory, GarbageCollector) *, DefaultCollector, ()),
        OPERATION(Alloc, (void *outPointerObject, size_t size)),
        OPERATION(AllocTyped, (void *outPointerObject, const MODULE_NS(Memory, GarbageCollector, Descriptor) *descriptor)),
        OPERATION(AllocAtomic, (void *outPointerObject, size_t size)),
        OPERATION(SetFinalizer, (const void *ptr, GARBAGE_COLLECTOR_FINALIZER_CALLBACK finalizer)),
        OPERATION(PinRoot, (const void *rootPointerAddress)),
        OPERATION(UnpinRoot, (const void *rootPointerAddress)),
        OPERATION(Collect, ()),
        PROCEDURE(Pause, ()),
        PROCEDURE(Resume, ()),
        FUNCTION(size_t, LiveBytes, ()),
        PREDICATE(Contains, (const void *ptr))
    )
);

typedef MODULE_TYPE(Memory, GarbageCollector) TMemoryGarbageCollector;
typedef MODULE_TYPE(Memory, GarbageCollector) TGarbageCollector;

#include "Impl/GarbageCollector.impl"

/* EOF */
