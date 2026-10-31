#pragma once

#include "../Error/Status.h"
#include "../Module/Object.h"
#include "../Preprocessor/Compiling.inc"
#include "Alloc.h"
#include "Memory.space"

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <string.h>

#define HEAP_MOD GNS2(MEMORY_MOD, Heap)
#define HEAP_CMOD PP_OP_CAT3(MEMORY_CMOD, _, HEAP)

#define HEAP_NS(NAME) GNS3(LIB_PREFIX(MEMORY_MOD), Heap, NAME)
#define HEAP_CNS(NAME) CNS3(LIB_PREFIX_CONST(MEMORY_CMOD), HEAP, NAME)
#define HEAP_INS(NAME) GNS4(LIB_PREFIX(MEMORY_MOD), Heap, Internal, NAME)
#define HEAP_CINS(NAME) CNS4(LIB_PREFIX_CONST(MEMORY_CMOD), HEAP, INTERNAL, NAME)

#define HEAP_TYPE(NAME) HEAP_NS(NAME)
#define HEAP_FUNC(NAME) GNS2(HEAP_TYPE(TMemoryHeap), NAME)
#define HEAP_CONST(NAME) HEAP_CNS(NAME)

/* ============================================================
 * 1. X-Macro Tables and Constants
 * ============================================================ */

#define HEAP_POLICY_COL_NAME(NAME, VALUE, DESC) NAME
#define HEAP_POLICY_COL_VALUE(NAME, VALUE, DESC) VALUE
#define HEAP_POLICY_COL_DESC(NAME, VALUE, DESC) DESC
#define HEAP_POLICY_GET(FIELD, ROW) PP_OP_CAT2(HEAP_POLICY_COL_, FIELD) ROW

#define HEAP_POLICY_TABLE(X) \
  X((CONSERVATIVE, 0U, "Conservative pointer scanning")) \
  X((TYPED,        1U, "Precise scanning guided by descriptor bitmap")) \
  X((ATOMIC,       2U, "Leaf object without internal pointers"))

typedef enum HEAP_TYPE(Policy) {
#define HEAP_POLICY_ENUM_ITEM(ROW) \
  HEAP_CONST(PP_OP_CAT2(POLICY_, HEAP_POLICY_GET(NAME, ROW))) = HEAP_POLICY_GET(VALUE, ROW),
  HEAP_POLICY_TABLE(HEAP_POLICY_ENUM_ITEM)
#undef HEAP_POLICY_ENUM_ITEM
} HEAP_TYPE(Policy);

#define HEAP_PAGE_STATE_COL_NAME(NAME, VALUE, DESC) NAME
#define HEAP_PAGE_STATE_COL_VALUE(NAME, VALUE, DESC) VALUE
#define HEAP_PAGE_STATE_COL_DESC(NAME, VALUE, DESC) DESC
#define HEAP_PAGE_STATE_GET(FIELD, ROW) PP_OP_CAT2(HEAP_PAGE_STATE_COL_, FIELD) ROW

#define HEAP_PAGE_STATE_TABLE(X) \
  X((ACTIVE,      0U, "Page has free cells available")) \
  X((FULL,        1U, "All cells currently allocated")) \
  X((NEEDS_SWEEP, 2U, "Page pending lazy sweep")) \
  X((FREE,        3U, "Page completely reclaimed and free"))

typedef enum HEAP_TYPE(PageState) {
#define HEAP_PAGE_STATE_ENUM_ITEM(ROW) \
  HEAP_CONST(PP_OP_CAT2(PAGE_STATE_, HEAP_PAGE_STATE_GET(NAME, ROW))) = HEAP_PAGE_STATE_GET(VALUE, ROW),
  HEAP_PAGE_STATE_TABLE(HEAP_PAGE_STATE_ENUM_ITEM)
#undef HEAP_PAGE_STATE_ENUM_ITEM
} HEAP_TYPE(PageState);

#define HEAP_CONFIG_COL_NAME(NAME, VALUE) NAME
#define HEAP_CONFIG_COL_VALUE(NAME, VALUE) VALUE
#define HEAP_CONFIG_GET(FIELD, ROW) PP_OP_CAT2(HEAP_CONFIG_COL_, FIELD) ROW

#define HEAP_CONFIG_TABLE(X) \
  X((PAGE_SIZE,           4096U)) \
  X((PAGE_SHIFT,          12U))   \
  X((MAX_SMALL_SIZE,      2048U)) \
  X((SIZE_CLASS_COUNT,    24U))   \
  X((BITMAP_WORDS,        4U))    \
  X((RADIX_LEVEL2_BITS,   12U))   \
  X((RADIX_BUCKETS_COUNT, 256U))  \
  X((PAGE_HEADER_SIZE,    128U))  \
  X((MAX_BLACKLISTED,     256U))

enum HEAP_TYPE(Config) {
#define HEAP_CONFIG_ENUM_ITEM(ROW) \
  HEAP_CONST(HEAP_CONFIG_GET(NAME, ROW)) = HEAP_CONFIG_GET(VALUE, ROW),
  HEAP_CONFIG_TABLE(HEAP_CONFIG_ENUM_ITEM)
#undef HEAP_CONFIG_ENUM_ITEM
};

/* ============================================================
 * 2. Internal BiBOP and Radix Structures
 * ============================================================ */

typedef struct HEAP_INS(PageHeader) {
  uint8_t isLargeSpan;
  uint8_t policy;
  uint8_t state;
  uint8_t _reserved;
  uint16_t sizeClassIndex;
  uint16_t elementSize;
  uint16_t cellCount;
  uint16_t freeCellCount;
  uint16_t firstFreeCell;
  struct HEAP_INS(PageHeader) *next;
  struct HEAP_INS(PageHeader) *prev;
  void *rawBlock;
  const void *descriptor;
  uint64_t allocBitmap[HEAP_CONST(BITMAP_WORDS)];
  uint64_t markBitmap[HEAP_CONST(BITMAP_WORDS)];
  uint8_t _padding[128 - (4 * sizeof(void *) + 8 * sizeof(uint64_t) + 16)];
  uint8_t payload[];
} HEAP_INS(PageHeader);

typedef struct HEAP_INS(Span) {
  uint8_t isLargeSpan;
  uint8_t policy;
  uint8_t isMarked;
  uint8_t _reserved;
  struct HEAP_INS(Span) *next;
  struct HEAP_INS(Span) *prev;
  size_t pageCount;
  size_t size;
  const void *descriptor;
  void *rawBlock;
  uint8_t data[];
} HEAP_INS(Span);

typedef struct HEAP_INS(RadixLevel2) {
  uintptr_t l1Key;
  void *pages[1U << HEAP_CONST(RADIX_LEVEL2_BITS)];
  struct HEAP_INS(RadixLevel2) *next;
} HEAP_INS(RadixLevel2);

typedef struct HEAP_INS(SizeClass) {
  HEAP_INS(PageHeader) *activePages;
  HEAP_INS(PageHeader) *fullPages;
  size_t elementSize;
} HEAP_INS(SizeClass);

/* ============================================================
 * 3. Declarative TMemoryHeap Object
 * ============================================================ */

OBJECT(HEAP_TYPE(TMemoryHeap),
    FIELDS(
        HEAP_INS(RadixLevel2) *radixBuckets[HEAP_CONST(RADIX_BUCKETS_COUNT)];
        HEAP_INS(SizeClass) classes[HEAP_CONST(SIZE_CLASS_COUNT)];
        HEAP_INS(Span) *largeObjects;
        HEAP_INS(PageHeader) *freePages;
        uintptr_t minAddress;
        uintptr_t maxAddress;
        size_t totalAllocatedBytes;
        size_t liveBytes;
        size_t pageCount;
        uintptr_t blacklistedPages[HEAP_CONST(MAX_BLACKLISTED)];
        size_t blacklistedCount;
    ),
    OPERATION(Create, (size_t initialPages)),
    OPERATION(Destroy, ()),
    OPERATION(AllocSmall, (void *outPointerObject, size_t sizeClassIndex, uint8_t policy, const void *descriptor)),
    OPERATION(AllocLarge, (void *outPointerObject, size_t size, uint8_t policy, const void *descriptor)),
    OPERATION(Alloc, (void *outPointerObject, size_t size, uint8_t policy, const void *descriptor)),
    OPERATION(FreeCell, (void *ptr)),
    OPERATION(BlacklistPage, (uintptr_t pageIndex)),
    CONST_PREDICATE(IsBlacklisted, (uintptr_t pageIndex)),
    CONST_FUNCTION(void *, LookupEntry, (const void *ptr)),
    CONST_PREDICATE(ResolvePointer, (const void *interiorPtr, void **outBasePtr, size_t *outSize, uint8_t *outPolicy)),
    OPERATION(SweepPage, (HEAP_INS(PageHeader) *page, bool *outIsPageEmpty)),
    OPERATION(LazySweepClass, (size_t sizeClassIndex)),
    CONST_FUNCTION_INLINE(size_t, TotalAllocated, (), { return self ? self->totalAllocatedBytes : 0U; }),
    CONST_FUNCTION_INLINE(size_t, LiveBytes, (), { return self ? self->liveBytes : 0U; }),
    CONST_FUNCTION_INLINE(size_t, PageCount, (), { return self ? self->pageCount : 0U; }),
    CONST_PREDICATE(Contains, (const void *ptr))
);

typedef HEAP_TYPE(TMemoryHeap) TMemoryHeap;

#include "Impl/Heap.impl"
/* EOF */

