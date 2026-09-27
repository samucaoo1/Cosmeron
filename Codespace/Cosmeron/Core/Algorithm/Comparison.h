#pragma once

#include "../Preprocessor/Compiling.inc"
#include "Algorithm.space"

#include <stddef.h>
#include <string.h>

#define COMPARISON_TYPE(NAME) GNS2(LIB_PREFIX(Comparison), NAME)
#define COMPARISON_FUNC(NAME) GNS2(LIB_PREFIX(Comparison), NAME)
#define COMPARISON_CONST(NAME) CNS2(LIB_PREFIX_CONST(COMPARISON), NAME)

typedef enum COMPARISON_TYPE(Result) {
  COMPARISON_CONST(LOWER) = -1,
  COMPARISON_CONST(EQUAL) = 0,
  COMPARISON_CONST(HIGHER) = 1
} TComparisonResult;

typedef TComparisonResult (*TComparator)(const void *left, const void *right);

#define COMPARISON_TABLE(X)                                                    \
  X(COMPARISON_CONST(LOWER))                                                   \
  X(COMPARISON_CONST(EQUAL))                                                   \
  X(COMPARISON_CONST(HIGHER))

#define COMPARISON_DECLARE(TYPE, SUFFIX)                                       \
  static inline TComparisonResult COMPARISON_FUNC(SUFFIX)(TYPE left, TYPE right)

#define COMPARISON_DECLARE_STRING(TYPE, SUFFIX)                                \
  static inline TComparisonResult COMPARISON_FUNC(SUFFIX)(TYPE left, TYPE right)

#define COMPARISON_BYTES_PROTOTYPE                                             \
  static inline TComparisonResult COMPARISON_FUNC(Bytes)(                      \
      const void *left, size_t leftSize, const void *right, size_t rightSize)

#define COMPARISON_INVOKE_PROTOTYPE                                            \
  static inline TComparisonResult COMPARISON_FUNC(Invoke)(                     \
      const void *left, const void *right, TComparator comparator)

COMPARISON_BYTES_PROTOTYPE;
COMPARISON_INVOKE_PROTOTYPE;

#include "Impl/Comparison.impl"

#define Comparison_Compare(TYPE, left, right)                                  \
  COMPARISON_FUNC(TYPE)((left), (right))
/* EOF */
