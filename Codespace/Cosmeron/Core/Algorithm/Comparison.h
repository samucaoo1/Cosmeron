#pragma once

#include "../Preprocessor/Compiling.inc"
#include "Algorithm.space"

#include <stddef.h>
#include <string.h>

#define COMPARISON_MOD Comparison
#define COMPARISON_CMOD COMPARISON

#define COMPARISON_NS(NAME) GNS2(LIB_PREFIX(COMPARISON_MOD), NAME)
#define COMPARISON_CNS(NAME) CNS2(LIB_PREFIX_CONST(COMPARISON_CMOD), NAME)
#define COMPARISON_INS(NAME) GNS3(LIB_PREFIX(COMPARISON_MOD), Internal, NAME)
#define COMPARISON_CINS(NAME) CNS3(LIB_PREFIX_CONST(COMPARISON_CMOD), INTERNAL, NAME)

#define COMPARISON_TYPE(NAME) COMPARISON_NS(NAME)
#define COMPARISON_FUNC(NAME) COMPARISON_NS(NAME)
#define COMPARISON_CONST(NAME) COMPARISON_CNS(NAME)

typedef enum COMPARISON_TYPE(Result) {
  COMPARISON_CONST(RESULT_LOWER) = -1,
  COMPARISON_CONST(RESULT_EQUAL) = 0,
  COMPARISON_CONST(RESULT_HIGHER) = 1
} COMPARISON_TYPE(Result);

typedef COMPARISON_TYPE(Result) CMPOUT;

typedef CMPOUT (*COMPARISON_TYPE(Comparator))(const void *left, const void *right);

#define COMPARISON_TABLE(X)                                                    \
  X(COMPARISON_CONST(RESULT_LOWER))                                                   \
  X(COMPARISON_CONST(RESULT_EQUAL))                                                   \
  X(COMPARISON_CONST(RESULT_HIGHER))

#define COMPARISON_TYPED_PROTOTYPE(TYPE, SUFFIX)                               \
  static inline CMPOUT COMPARISON_FUNC(SUFFIX)(TYPE left, TYPE right)

#define COMPARISON_STRING_PROTOTYPE(TYPE, SUFFIX)                              \
  static inline CMPOUT COMPARISON_FUNC(SUFFIX)(TYPE left, TYPE right)

#define COMPARISON_BYTES_PROTOTYPE                                             \
  static inline CMPOUT COMPARISON_FUNC(Bytes)(                                 \
      const void *left, size_t leftSize, const void *right, size_t rightSize)

#define COMPARISON_INVOKE_PROTOTYPE                                            \
  static inline CMPOUT COMPARISON_FUNC(Invoke)(                                \
      const void *left, const void *right, COMPARISON_TYPE(Comparator) comparator)

COMPARISON_BYTES_PROTOTYPE;
COMPARISON_INVOKE_PROTOTYPE;

#include "Impl/Comparison.impl"

#define Comparison_Compare(TYPE, left, right)                                  \
  COMPARISON_FUNC(TYPE)((left), (right))
/* EOF */
