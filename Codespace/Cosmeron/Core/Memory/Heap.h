#pragma once

#include "../Outcome/Status.h"
#include "../Module/Module.h"
#include "../Preprocessor/Compiling.inc"
#include "../Preprocessor/Operations.h"
#include "Alloc.h"
#include "Common.inc"

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

/* ========================================================================== */
/* 1. X-Macro Configuration Tables and Enums                                  */
/* ========================================================================== */

#define HEAP_POLICY_COL_NAME(NAME, VALUE) NAME
#define HEAP_POLICY_COL_VALUE(NAME, VALUE) VALUE
#define HEAP_POLICY_GET(FIELD, ROW) PP_OP_CAT2(HEAP_POLICY_COL_, FIELD) ROW

#define HEAP_POLICY_TABLE(X) \
  X((MANUAL,       0U)) \
  X((CONSERVATIVE, 1U)) \
  X((TYPED,        2U)) \
  X((ATOMIC,       3U)) \
  X((IMMUTABLE,    4U)) \
  X((TRACED,       5U)) \
  X((PINNED,       6U))

#define HEAP_POLICY_DISPATCH(TARGET, ENUM_NAME, ROW) \
  MODULE_CONST(MEMORY, HEAP, POLICY, HEAP_POLICY_GET(NAME, ROW)) = HEAP_POLICY_GET(VALUE, ROW),

ENUM_DEF_EX(MODULE_NS(Memory, Heap), Policy, HEAP_POLICY_TABLE, HEAP_POLICY_DISPATCH)

#define HEAP_PAGE_STATE_COL_NAME(NAME, VALUE, DESC) NAME
#define HEAP_PAGE_STATE_COL_VALUE(NAME, VALUE, DESC) VALUE
#define HEAP_PAGE_STATE_COL_DESC(NAME, VALUE, DESC) DESC
#define HEAP_PAGE_STATE_GET(FIELD, ROW) PP_OP_CAT2(HEAP_PAGE_STATE_COL_, FIELD) ROW

#define HEAP_PAGE_STATE_TABLE(X) \
  X((ACTIVE,      0U, "Page has free cells available")) \
  X((FULL,        1U, "All cells currently allocated")) \
  X((NEEDS_SWEEP, 2U, "Page pending lazy sweep")) \
  X((FREE,        3U, "Page completely reclaimed and free"))

#define HEAP_PAGE_STATE_DISPATCH(TARGET, ENUM_NAME, ROW) \
  MODULE_CONST(MEMORY, HEAP, PAGE_STATE, HEAP_PAGE_STATE_GET(NAME, ROW)) = HEAP_PAGE_STATE_GET(VALUE, ROW),

ENUM_DEF_EX(MODULE_NS(Memory, Heap), PageState, HEAP_PAGE_STATE_TABLE, HEAP_PAGE_STATE_DISPATCH)

#define HEAP_CONFIG_COL_NAME(NAME, VALUE) NAME
#define HEAP_CONFIG_COL_VALUE(NAME, VALUE) VALUE
#define HEAP_CONFIG_GET(FIELD, ROW) PP_OP_CAT2(HEAP_CONFIG_COL_, FIELD) ROW

#define HEAP_CONFIG_TABLE(X) \
  X((PAGE_SIZE,           4096U)) \
  X((PAGE_MASK,           4095U)) \
  X((PAGE_SHIFT,          12U))   \
  X((MAX_SMALL_SIZE,      2048U)) \
  X((SIZE_CLASS_COUNT,    24U))   \
  X((BITMAP_WORDS,        4U))    \
  X((RADIX_LEVEL2_BITS,   12U))   \
  X((RADIX_BUCKETS_COUNT, 256U))  \
  X((PAGE_HEADER_SIZE,    128U))  \
  X((MAX_BLACKLISTED,     256U))

#define HEAP_CONFIG_DISPATCH(TARGET, ENUM_NAME, ROW) \
  MODULE_CONST(MEMORY, HEAP, HEAP_CONFIG_GET(NAME, ROW)) = HEAP_CONFIG_GET(VALUE, ROW),

ENUM_DEF_EX(MODULE_NS(Memory, Heap), Config, HEAP_CONFIG_TABLE, HEAP_CONFIG_DISPATCH)

/* ========================================================================== */
/* 2. Internal BiBOP and Radix Structures                                     */
/* ========================================================================== */

typedef struct MODULE_ITYPE(Memory, HeapPageHeader) {
  uint8_t isLargeSpan;
  uint8_t policy;
  uint8_t state;
  uint8_t _reserved;
  uint16_t sizeClassIndex;
  uint16_t elementSize;
  uint16_t cellCount;
  uint16_t freeCellCount;
  uint16_t firstFreeCell;
  struct MODULE_ITYPE(Memory, HeapPageHeader) *next;
  struct MODULE_ITYPE(Memory, HeapPageHeader) *prev;
  void *rawBlock;
  const void *descriptor;
  uint64_t allocBitmap[MODULE_CONST(MEMORY, HEAP, BITMAP_WORDS)];
  uint64_t markBitmap[MODULE_CONST(MEMORY, HEAP, BITMAP_WORDS)];
  uint8_t _padding[128 - (4 * sizeof(void *) + 8 * sizeof(uint64_t) + 16)];
  uint8_t payload[];
} MODULE_ITYPE(Memory, HeapPageHeader);

typedef struct MODULE_ITYPE(Memory, HeapSpan) {
  uint8_t isLargeSpan;
  uint8_t policy;
  uint8_t isMarked;
  uint8_t _reserved;
  struct MODULE_ITYPE(Memory, HeapSpan) *next;
  struct MODULE_ITYPE(Memory, HeapSpan) *prev;
  size_t pageCount;
  size_t size;
  const void *descriptor;
  void *rawBlock;
  uint8_t data[];
} MODULE_ITYPE(Memory, HeapSpan);

typedef struct MODULE_ITYPE(Memory, HeapRadixLevel2) {
  uintptr_t l1Key;
  void *pages[1U << MODULE_CONST(MEMORY, HEAP, RADIX_LEVEL2_BITS)];
  struct MODULE_ITYPE(Memory, HeapRadixLevel2) *next;
} MODULE_ITYPE(Memory, HeapRadixLevel2);

typedef struct MODULE_ITYPE(Memory, HeapSizeClass) {
  MODULE_ITYPE(Memory, HeapPageHeader) *activePages;
  MODULE_ITYPE(Memory, HeapPageHeader) *fullPages;
  size_t elementSize;
} MODULE_ITYPE(Memory, HeapSizeClass);

/* ========================================================================== */
/* 3. Declarative TMemoryHeap Object                                          */
/* ========================================================================== */

MODULE(DOMAIN(Core), Memory,
    OBJECT(Heap,
        FIELDS(
            MODULE_ITYPE(Memory, HeapRadixLevel2) *radixBuckets[MODULE_CONST(MEMORY, HEAP, RADIX_BUCKETS_COUNT)];
            MODULE_ITYPE(Memory, HeapSizeClass) classes[MODULE_CONST(MEMORY, HEAP, SIZE_CLASS_COUNT)];
            MODULE_ITYPE(Memory, HeapSpan) *largeObjects;
            MODULE_ITYPE(Memory, HeapPageHeader) *freePages;
            uintptr_t minAddress;
            uintptr_t maxAddress;
            size_t totalAllocatedBytes;
            size_t liveBytes;
            size_t pageCount;
            uintptr_t blacklistedPages[MODULE_CONST(MEMORY, HEAP, MAX_BLACKLISTED)];
            size_t blacklistedCount;
        ),
        OPERATION(Create, (size_t initialPages)),
        OPERATION(Destroy, ()),
        VARIANT(Alloc,
            OPERATION((void *outPointerObject, size_t size, uint8_t policy, const void *descriptor)),
            OPERATION(Small, (void *outPointerObject, size_t sizeClassIndex, uint8_t policy, const void *descriptor)),
            OPERATION(Large, (void *outPointerObject, size_t size, uint8_t policy, const void *descriptor))
        ),
        OPERATION(FreeCell, (void *ptr)),
        OPERATION(BlacklistPage, (uintptr_t pageIndex)),
        CONST_PREDICATE(IsBlacklisted, (uintptr_t pageIndex)),
        CONST_FUNCTION(void *, LookupEntry, (const void *ptr)),
        CONST_PREDICATE(ResolvePointer, (const void *interiorPtr, void **outBasePtr, size_t *outSize, uint8_t *outPolicy)),
        OPERATION(SweepPage, (MODULE_ITYPE(Memory, HeapPageHeader) *page, bool *outIsPageEmpty)),
        OPERATION(LazySweepClass, (size_t sizeClassIndex)),
        CONST_FUNCTION_INLINE(size_t, TotalAllocated, (), { return self ? self->totalAllocatedBytes : 0U; }),
        CONST_FUNCTION_INLINE(size_t, LiveBytes, (), { return self ? self->liveBytes : 0U; }),
        CONST_FUNCTION_INLINE(size_t, PageCount, (), { return self ? self->pageCount : 0U; }),
        CONST_PREDICATE(Contains, (const void *ptr))
    )
);

typedef MODULE_TYPE(Memory, Heap) TMemoryHeap;

#include "Impl/Heap.impl"

/* EOF */
