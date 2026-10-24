#pragma once

#include "../Preprocessor/Compiling.inc"
#include "Algorithm.space"

#include <stddef.h>

#define COMPARISON_MOD Comparison
#define COMPARISON_CMOD COMPARISON

#define COMPARISON_NS(NAME) GNS2(ALGORITHM_NS(COMPARISON_MOD), NAME)
#define COMPARISON_CNS(NAME)                                                   \
  CNS3(LIB_PREFIX_CONST(ALGORITHM_CMOD), COMPARISON_CMOD, NAME)
#define COMPARISON_INS(NAME) GNS3(ALGORITHM_NS(COMPARISON_MOD), Internal, NAME)
#define COMPARISON_CINS(NAME)                                                  \
  CNS3(ALGORITHM_CNS(COMPARISON_CMOD), INTERNAL, NAME)

#define COMPARISON_TYPE(NAME) COMPARISON_NS(NAME)
#define COMPARISON_FUNC(NAME) COMPARISON_NS(NAME)
#define COMPARISON_CONST(NAME) COMPARISON_CNS(NAME)

#define COMPARISON_TABLE(X)                                                    \
  X(LOWER, -1, "Lower")                                                        \
  X(EQUAL, 0, "Equal")                                                         \
  X(HIGHER, 1, "Higher")

typedef enum COMPARISON_TYPE(Result) {
#define X(NAME, VALUE, DESC) COMPARISON_CONST(NAME) = VALUE,
  COMPARISON_TABLE(X)
#undef X
} COMPARISON_TYPE(Result);

typedef COMPARISON_TYPE(Result) CMPOUT;

typedef CMPOUT (*COMPARISON_TYPE(Comparator))(const void *left,
                                              const void *right);

typedef CMPOUT (*COMPARISON_TYPE(ContextComparator))(const void *left,
                                                     const void *right,
                                                     void *context);

#include "../Module/Package.h"
#include "../Primitive/Types.h"

#define COMPARISON_PRIMITIVE_FUNCTION_DECL(TYPE, LONG_NAME, SHORT_NAME,        \
                                           T_PATTERN, CATEGORY, BITS)          \
  FUNCTION(CMPOUT, LONG_NAME, (TYPE left, TYPE right)),

PACKAGE(ALGORITHM_NS(COMPARISON_MOD),
        PRIMITIVE_TYPES_TABLE(COMPARISON_PRIMITIVE_FUNCTION_DECL)
            FUNCTION(CMPOUT, CString, (const char *left, const char *right)),
        FUNCTION(CMPOUT, Bytes,
                 (const void *left, size_t leftSize, const void *right,
                  size_t rightSize)),
        FUNCTION(CMPOUT, Invoke,
                 (const void *left, const void *right,
                  COMPARISON_TYPE(Comparator) comparator)));

#undef COMPARISON_PRIMITIVE_FUNCTION_DECL

#include "Impl/Comparison.impl"
/* EOF */
