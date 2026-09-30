#pragma once

#include "../Preprocessor/Compiling.inc"
#include "Algorithm.space"

#include <stddef.h>
#include <string.h>

#define COMPARISON_NS(NAME) GNS2(ALGORITHM_NS(Comparison), NAME)
#define COMPARISON_CNS(NAME)                                                  \
  CNS3(LIB_PREFIX_CONST(ALGORITHM_CMOD), COMPARISON, NAME)
#define COMPARISON_INS(NAME)                                                  \
  GNS4(LIB_PREFIX(ALGORITHM_MOD), Comparison, Internal, NAME)
#define COMPARISON_CINS(NAME)                                                 \
  CNS4(LIB_PREFIX_CONST(ALGORITHM_CMOD), COMPARISON, INTERNAL, NAME)

#define COMPARISON_TYPE(NAME) COMPARISON_NS(NAME)
#define COMPARISON_FUNC(NAME) COMPARISON_NS(NAME)
#define COMPARISON_CONST(NAME) COMPARISON_CNS(NAME)

#define COMPARISON_TABLE(X)                                                   \
  X(COMPARISON_CONST(LESS),-1, "Less than")                                                   \
  X(COMPARISON_CONST(SAME), 0, "Same")                                                   \
  X(COMPARISON_CONST(GREATER), 1, "Greater than")

typedef enum COMPARISON_TYPE(Result) {
#define X(NAME, VALUE, DESC) NAME = VALUE,
  COMPARISON_TABLE(X)
#undef X
} COMPARISON_TYPE(Result);

typedef COMPARISON_TYPE(Result) CMPOUT;

typedef CMPOUT (*COMPARISON_TYPE(Comparator))(const void *left, const void *right);

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
