#pragma once

#include "Flat.space"
#include "../../../Core/Algorithm/Comparison.h"

/* ============================================================
 * Flat_Heap (Binary Min/Max Heap Priority Queue)
 * ============================================================ */

#define FLAT_HEAP_TYPE(SUFFIX) GNS2(FLAT_NS(THeap), SUFFIX)
#define FLAT_HEAP_NS(SUFFIX) GNS2(FLAT_NS(Heap), SUFFIX)
#define FLAT_HEAP_FUNC(SUFFIX, FUNC) GNS2(FLAT_HEAP_NS(SUFFIX), FUNC)
#define FLAT_HEAP_STRUCT_TAG(SUFFIX) GNS2(FLAT_HEAP_NS(SUFFIX), str)
#define FLAT_HEAP_FUNCTION_TABLE_TYPE(SUFFIX)                                  \
  GNS2(FLAT_HEAP_NS(SUFFIX), FunctionTable)

#define FLAT_HEAP_STRUCT(TYPE, SUFFIX)                                         \
  CONTAINER_API_FORWARD(FLAT_HEAP_FUNCTION_TABLE_TYPE(SUFFIX))                 \
  typedef struct FLAT_HEAP_STRUCT_TAG(SUFFIX) {                                \
    CONTAINER_API_FIELD(FLAT_HEAP_FUNCTION_TABLE_TYPE(SUFFIX))                 \
    TYPE *data;                                                                \
    size_t size;                                                               \
    size_t capacity;                                                           \
    CMPOUT (*compare)(TYPE left, TYPE right);                                  \
  } FLAT_HEAP_TYPE(SUFFIX);

/* ============================================================
 * Prototypes
 * ============================================================ */

#define FLAT_HEAP_INIT_PROTOTYPE(SUFFIX)                                       \
  static inline OPSTATUS FLAT_HEAP_FUNC(SUFFIX, Init)(                         \
      FLAT_HEAP_TYPE(SUFFIX) * heap)

#define FLAT_HEAP_INIT_MAX_PROTOTYPE(SUFFIX)                                   \
  static inline OPSTATUS FLAT_HEAP_FUNC(SUFFIX, InitMax)(                      \
      FLAT_HEAP_TYPE(SUFFIX) * heap)

#define FLAT_HEAP_INIT_WITH_COMPARATOR_PROTOTYPE(TYPE, SUFFIX)                 \
  static inline OPSTATUS FLAT_HEAP_FUNC(SUFFIX, InitWithComparator)(           \
      FLAT_HEAP_TYPE(SUFFIX) * heap, CMPOUT (*compare)(TYPE left, TYPE right))

#define FLAT_HEAP_DESTROY_PROTOTYPE(SUFFIX)                                    \
  static inline void FLAT_HEAP_FUNC(SUFFIX, Destroy)(                          \
      FLAT_HEAP_TYPE(SUFFIX) * heap)

#define FLAT_HEAP_CLEAR_PROTOTYPE(SUFFIX)                                      \
  static inline void FLAT_HEAP_FUNC(SUFFIX, Clear)(                            \
      FLAT_HEAP_TYPE(SUFFIX) * heap)

#define FLAT_HEAP_RESERVE_PROTOTYPE(SUFFIX)                                    \
  static inline OPSTATUS FLAT_HEAP_FUNC(SUFFIX, Reserve)(                      \
      FLAT_HEAP_TYPE(SUFFIX) * heap, size_t newCapacity)

#define FLAT_HEAP_SHRINK_TO_FIT_PROTOTYPE(SUFFIX)                              \
  static inline OPSTATUS FLAT_HEAP_FUNC(SUFFIX, ShrinkToFit)(                  \
      FLAT_HEAP_TYPE(SUFFIX) * heap)

#define FLAT_HEAP_PUSH_PROTOTYPE(TYPE, SUFFIX)                                 \
  static inline OPSTATUS FLAT_HEAP_FUNC(SUFFIX, Push)(                         \
      FLAT_HEAP_TYPE(SUFFIX) * heap, TYPE value)

#define FLAT_HEAP_POP_PROTOTYPE(TYPE, SUFFIX)                                  \
  static inline OPSTATUS FLAT_HEAP_FUNC(SUFFIX, Pop)(                          \
      FLAT_HEAP_TYPE(SUFFIX) * heap, TYPE * outValue)

#define FLAT_HEAP_PEEK_PROTOTYPE(TYPE, SUFFIX)                                 \
  static inline OPSTATUS FLAT_HEAP_FUNC(SUFFIX, Peek)(                         \
      const FLAT_HEAP_TYPE(SUFFIX) * heap, TYPE * outValue)

#define FLAT_HEAP_TOP_PROTOTYPE(TYPE, SUFFIX)                                  \
  static inline OPSTATUS FLAT_HEAP_FUNC(SUFFIX, Top)(                          \
      FLAT_HEAP_TYPE(SUFFIX) * heap, TYPE **out)

#define FLAT_HEAP_REPLACE_PROTOTYPE(TYPE, SUFFIX)                              \
  static inline OPSTATUS FLAT_HEAP_FUNC(SUFFIX, Replace)(                      \
      FLAT_HEAP_TYPE(SUFFIX) * heap, TYPE inValue, TYPE * outValue)

#define FLAT_HEAP_PUSH_POP_PROTOTYPE(TYPE, SUFFIX)                             \
  static inline OPSTATUS FLAT_HEAP_FUNC(SUFFIX, PushPop)(                      \
      FLAT_HEAP_TYPE(SUFFIX) * heap, TYPE inValue, TYPE * outValue)

#define FLAT_HEAP_HEAPIFY_PROTOTYPE(SUFFIX)                                    \
  static inline OPSTATUS FLAT_HEAP_FUNC(SUFFIX, Heapify)(                      \
      FLAT_HEAP_TYPE(SUFFIX) * heap)

#define FLAT_HEAP_FROM_BUFFER_PROTOTYPE(TYPE, SUFFIX)                          \
  static inline OPSTATUS FLAT_HEAP_FUNC(SUFFIX, FromBuffer)(                   \
      FLAT_HEAP_TYPE(SUFFIX) * heap, const TYPE * buffer, size_t count)

#define FLAT_HEAP_EMPTY_PROTOTYPE(SUFFIX)                                      \
  static inline bool FLAT_HEAP_FUNC(SUFFIX, Empty)(                            \
      const FLAT_HEAP_TYPE(SUFFIX) * heap)

#define FLAT_HEAP_SIZE_PROTOTYPE(SUFFIX)                                       \
  static inline size_t FLAT_HEAP_FUNC(SUFFIX, Size)(                           \
      const FLAT_HEAP_TYPE(SUFFIX) * heap)

#define FLAT_HEAP_CAPACITY_PROTOTYPE(SUFFIX)                                   \
  static inline size_t FLAT_HEAP_FUNC(SUFFIX, Capacity)(                       \
      const FLAT_HEAP_TYPE(SUFFIX) * heap)

#include "FunctionTable/Heap.h"

#define FLAT_HEAP_DEFAULT_COMPARISON_HELPERS(TYPE, SUFFIX)                     \
  static inline CMPOUT FLAT_HEAP_FUNC(SUFFIX, DefaultMinCompare)(              \
      TYPE left, TYPE right) {                                                 \
    return (left < right) ? COMPARISON_LOWER_CONST                             \
                          : ((left > right) ? COMPARISON_HIGHER_CONST          \
                                            : COMPARISON_EQUAL_CONST);         \
  }                                                                            \
  static inline CMPOUT FLAT_HEAP_FUNC(SUFFIX, DefaultMaxCompare)(              \
      TYPE left, TYPE right) {                                                 \
    return (left > right) ? COMPARISON_LOWER_CONST                             \
                          : ((left < right) ? COMPARISON_HIGHER_CONST          \
                                            : COMPARISON_EQUAL_CONST);         \
  }

#define FLAT_HEAP_IMPLEMENT_ALL(TYPE, SUFFIX)                                  \
  FLAT_HEAP_STRUCT(TYPE, SUFFIX)                                               \
  FLAT_HEAP_DEFAULT_COMPARISON_HELPERS(TYPE, SUFFIX)                           \
  FLAT_HEAP_INIT_PROTOTYPE(SUFFIX);                                            \
  FLAT_HEAP_INIT_MAX_PROTOTYPE(SUFFIX);                                        \
  FLAT_HEAP_INIT_WITH_COMPARATOR_PROTOTYPE(TYPE, SUFFIX);                      \
  FLAT_HEAP_DESTROY_PROTOTYPE(SUFFIX);                                         \
  FLAT_HEAP_CLEAR_PROTOTYPE(SUFFIX);                                           \
  FLAT_HEAP_RESERVE_PROTOTYPE(SUFFIX);                                         \
  FLAT_HEAP_SHRINK_TO_FIT_PROTOTYPE(SUFFIX);                                   \
  FLAT_HEAP_PUSH_PROTOTYPE(TYPE, SUFFIX);                                      \
  FLAT_HEAP_POP_PROTOTYPE(TYPE, SUFFIX);                                       \
  FLAT_HEAP_PEEK_PROTOTYPE(TYPE, SUFFIX);                                      \
  FLAT_HEAP_TOP_PROTOTYPE(TYPE, SUFFIX);                                       \
  FLAT_HEAP_REPLACE_PROTOTYPE(TYPE, SUFFIX);                                   \
  FLAT_HEAP_PUSH_POP_PROTOTYPE(TYPE, SUFFIX);                                  \
  FLAT_HEAP_HEAPIFY_PROTOTYPE(SUFFIX);                                         \
  FLAT_HEAP_FROM_BUFFER_PROTOTYPE(TYPE, SUFFIX);                               \
  FLAT_HEAP_EMPTY_PROTOTYPE(SUFFIX);                                           \
  FLAT_HEAP_SIZE_PROTOTYPE(SUFFIX);                                            \
  FLAT_HEAP_CAPACITY_PROTOTYPE(SUFFIX);                                        \
  COSMERON_MACRO_INTERNAL_FLAT_HEAP_IMPLEMENT(TYPE, SUFFIX)                    \
  FLAT_HEAP_FUNCTION_TABLE_STRUCT(TYPE, SUFFIX)                                \
  FLAT_HEAP_FUNCTION_TABLE_INSTANCE(TYPE, SUFFIX)

#define FLAT_HEAP_IMPLEMENT_ALL_CMP(TYPE, SUFFIX, CMP)                         \
  FLAT_HEAP_STRUCT(TYPE, SUFFIX)                                               \
  static inline CMPOUT FLAT_HEAP_FUNC(SUFFIX, DefaultMinCompare)(              \
      TYPE left, TYPE right) {                                                 \
    return CMP(left, right);                                                   \
  }                                                                            \
  static inline CMPOUT FLAT_HEAP_FUNC(SUFFIX, DefaultMaxCompare)(              \
      TYPE left, TYPE right) {                                                 \
    CMPOUT res = CMP(left, right);                                             \
    return (res == COMPARISON_LOWER_CONST)                                     \
               ? COMPARISON_HIGHER_CONST                                       \
               : ((res == COMPARISON_HIGHER_CONST) ? COMPARISON_LOWER_CONST    \
                                                   : COMPARISON_EQUAL_CONST);  \
  }                                                                            \
  FLAT_HEAP_INIT_PROTOTYPE(SUFFIX);                                            \
  FLAT_HEAP_INIT_MAX_PROTOTYPE(SUFFIX);                                        \
  FLAT_HEAP_INIT_WITH_COMPARATOR_PROTOTYPE(TYPE, SUFFIX);                      \
  FLAT_HEAP_DESTROY_PROTOTYPE(SUFFIX);                                         \
  FLAT_HEAP_CLEAR_PROTOTYPE(SUFFIX);                                           \
  FLAT_HEAP_RESERVE_PROTOTYPE(SUFFIX);                                         \
  FLAT_HEAP_SHRINK_TO_FIT_PROTOTYPE(SUFFIX);                                   \
  FLAT_HEAP_PUSH_PROTOTYPE(TYPE, SUFFIX);                                      \
  FLAT_HEAP_POP_PROTOTYPE(TYPE, SUFFIX);                                       \
  FLAT_HEAP_PEEK_PROTOTYPE(TYPE, SUFFIX);                                      \
  FLAT_HEAP_TOP_PROTOTYPE(TYPE, SUFFIX);                                       \
  FLAT_HEAP_REPLACE_PROTOTYPE(TYPE, SUFFIX);                                   \
  FLAT_HEAP_PUSH_POP_PROTOTYPE(TYPE, SUFFIX);                                  \
  FLAT_HEAP_HEAPIFY_PROTOTYPE(SUFFIX);                                         \
  FLAT_HEAP_FROM_BUFFER_PROTOTYPE(TYPE, SUFFIX);                               \
  FLAT_HEAP_EMPTY_PROTOTYPE(SUFFIX);                                           \
  FLAT_HEAP_SIZE_PROTOTYPE(SUFFIX);                                            \
  FLAT_HEAP_CAPACITY_PROTOTYPE(SUFFIX);                                        \
  COSMERON_MACRO_INTERNAL_FLAT_HEAP_IMPLEMENT(TYPE, SUFFIX)                    \
  FLAT_HEAP_FUNCTION_TABLE_STRUCT(TYPE, SUFFIX)                                \
  FLAT_HEAP_FUNCTION_TABLE_INSTANCE(TYPE, SUFFIX)

#include "Impl/Heap.impl"

#define FLAT_HEAP_INSTANCE_DECLARE(TYPE, NAME)                                 \
  FLAT_HEAP_TYPE(TYPE) NAME;                                                   \
  CONTAINER_API_BIND(NAME, FLAT_HEAP_FUNC(TYPE, functions));                   \
  FLAT_HEAP_FUNC(TYPE, Init)(&NAME);

/* Compatibility alias: prefer FLAT_HEAP_INSTANCE_DECLARE. */
#define TFlat_Heap(TYPE, NAME) FLAT_HEAP_INSTANCE_DECLARE(TYPE, NAME)

FLAT_HEAP_IMPLEMENT_ALL(int, int)
FLAT_HEAP_IMPLEMENT_ALL(float, float)
FLAT_HEAP_IMPLEMENT_ALL(double, double)
