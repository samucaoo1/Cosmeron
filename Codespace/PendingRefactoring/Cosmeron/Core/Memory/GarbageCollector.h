#pragma once

#include "../Outcome/Status.h"
#include "../Module/Callback/Callback.h"
#include "../Module/Object.h"
#include "../Module/Package.h"
#include "../Preprocessor/Compiling.inc"
#include "../Preprocessor/Detect/OperationSystem.h"
#include "Alloc.h"
#include "Heap.h"
#include "Common.h"

#include <setjmp.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <string.h>

#define GARBAGE_COLLECTOR_NS(NAME) NS(LIB_PREFIX(Memory_GarbageCollector), NAME)
#define GARBAGE_COLLECTOR_CNS(NAME) CNS(LIB_PREFIX_CONST(MEMORY), GARBAGE_COLLECTOR, NAME)
#define GARBAGE_COLLECTOR_INS(NAME) NS(LIB_PREFIX(Memory_GarbageCollector), Internal, NAME)
#define GARBAGE_COLLECTOR_CINS(NAME) CNS(LIB_PREFIX_CONST(MEMORY), GARBAGE_COLLECTOR, INTERNAL, NAME)

#define GARBAGE_COLLECTOR_TYPE(NAME) PP_OP_CAT2(T, NAME)
#define GARBAGE_COLLECTOR_FUNC(NAME) NS(GARBAGE_COLLECTOR_TYPE(GarbageCollector), NAME)
#define GARBAGE_COLLECTOR_CONST(NAME) GARBAGE_COLLECTOR_CNS(NAME)

/* ============================================================
 * 1. Constants and Callbacks
 * ============================================================ */

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

enum GARBAGE_COLLECTOR_NS(Config) {
#define GARBAGE_COLLECTOR_CONFIG_ENUM_ITEM(ROW) \
  GARBAGE_COLLECTOR_CONST(GARBAGE_COLLECTOR_CONFIG_GET(NAME, ROW)) = \
      GARBAGE_COLLECTOR_CONFIG_GET(VALUE, ROW),
  GARBAGE_COLLECTOR_CONFIG_TABLE(GARBAGE_COLLECTOR_CONFIG_ENUM_ITEM)
#undef GARBAGE_COLLECTOR_CONFIG_ENUM_ITEM
};

CALLBACK_PROCEDURE(
    GARBAGE_COLLECTOR_INS(FinalizerFunction),
    (void *ptr)
);

#define GARBAGE_COLLECTOR_FINALIZER_CALLBACK \
  TCALLBACK(GARBAGE_COLLECTOR_INS(FinalizerFunction))

#define GARBAGE_COLLECTOR_FINALIZER_BIND(FN, CTX) \
  NS(CALLBACK_FUNC(GARBAGE_COLLECTOR_INS(FinalizerFunction)), Bind)((FN), (CTX))

/* Typed struct descriptor */
typedef struct GARBAGE_COLLECTOR_NS(Descriptor) {
  size_t size;
  uint64_t bitmap;
} GARBAGE_COLLECTOR_NS(Descriptor);

#define GARBAGE_COLLECTOR_FIELD_BIT(TYPE, FIELD) \
  (1ULL << (offsetof(TYPE, FIELD) / sizeof(void *)))

#define GARBAGE_COLLECTOR_DESCRIPTOR_DEF(NAME, TYPE, ...) \
  static const GARBAGE_COLLECTOR_NS(Descriptor) NAME = { \
      .size = sizeof(TYPE), \
      .bitmap = (0ULL | __VA_ARGS__) \
  }

/* ============================================================
 * 2. Internal Collector Data Structures
 * ============================================================ */

typedef struct GARBAGE_COLLECTOR_INS(WorklistChunk) {
  void *items[GARBAGE_COLLECTOR_CONST(WORKLIST_CHUNK_CAPACITY)];
  size_t count;
  struct GARBAGE_COLLECTOR_INS(WorklistChunk) *next;
} GARBAGE_COLLECTOR_INS(WorklistChunk);

typedef struct GARBAGE_COLLECTOR_INS(Worklist) {
  GARBAGE_COLLECTOR_INS(WorklistChunk) *head;
  GARBAGE_COLLECTOR_INS(WorklistChunk) *freeChunks;
  size_t totalPushed;
  bool overflow;
} GARBAGE_COLLECTOR_INS(Worklist);

typedef struct GARBAGE_COLLECTOR_INS(FinalizerEntry) {
  void *object;
  GARBAGE_COLLECTOR_FINALIZER_CALLBACK callback;
  struct GARBAGE_COLLECTOR_INS(FinalizerEntry) *next;
} GARBAGE_COLLECTOR_INS(FinalizerEntry);

typedef struct GARBAGE_COLLECTOR_INS(RootEntry) {
  const void *pointerAddress;
  bool active;
} GARBAGE_COLLECTOR_INS(RootEntry);

/* ============================================================
 * 3. Declarative TGarbageCollector Object
 * ============================================================ */

OBJECT(GARBAGE_COLLECTOR_TYPE(GarbageCollector),
    FIELDS(
        MEMORY_TYPE(Heap) heap;
        GARBAGE_COLLECTOR_INS(Worklist) worklist;
        GARBAGE_COLLECTOR_INS(FinalizerEntry) *finalizers;
        GARBAGE_COLLECTOR_INS(FinalizerEntry) *pendingFinalizers;
        GARBAGE_COLLECTOR_INS(RootEntry) roots[GARBAGE_COLLECTOR_CONST(MAX_ROOTS)];
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
    OPERATION(AllocTyped, (void *outPointerObject, const GARBAGE_COLLECTOR_NS(Descriptor) *descriptor)),
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
);

/* ============================================================
 * 4. Declarative Package GarbageCollector (Thread Default Facade)
 * ============================================================ */

PACKAGE(LIB_PREFIX(Memory_GarbageCollector),
    OPERATION(Startup, (const void *stackBottom)),
    OPERATION(Shutdown, ()),
    FUNCTION(GARBAGE_COLLECTOR_TYPE(GarbageCollector) *, DefaultCollector, ()),
    OPERATION(Alloc, (void *outPointerObject, size_t size)),
    OPERATION(AllocTyped, (void *outPointerObject, const GARBAGE_COLLECTOR_NS(Descriptor) *descriptor)),
    OPERATION(AllocAtomic, (void *outPointerObject, size_t size)),
    OPERATION(SetFinalizer, (const void *ptr, GARBAGE_COLLECTOR_FINALIZER_CALLBACK finalizer)),
    OPERATION(PinRoot, (const void *rootPointerAddress)),
    OPERATION(UnpinRoot, (const void *rootPointerAddress)),
    OPERATION(Collect, ()),
    PROCEDURE(Pause, ()),
    PROCEDURE(Resume, ()),
    FUNCTION(size_t, LiveBytes, ()),
    PREDICATE(Contains, (const void *ptr))
);

#include "Impl/GarbageCollector.impl"
/* EOF */

