#pragma once

#include "Distribution.space"
#include <limits.h>

#define RANDOM_DISTRIBUTION_U64_PROTOTYPE                                   \
  static inline uint64_t RANDOM_DISTRIBUTION_FUNC(U64)(                      \
      RANDOM_SOURCE_TYPE(Value) *source, uint64_t minimum, uint64_t maximum)

#define RANDOM_DISTRIBUTION_I64_PROTOTYPE                                   \
  static inline int64_t RANDOM_DISTRIBUTION_FUNC(I64)(                       \
      RANDOM_SOURCE_TYPE(Value) *source, int64_t minimum, int64_t maximum)

RANDOM_DISTRIBUTION_U64_PROTOTYPE;
RANDOM_DISTRIBUTION_I64_PROTOTYPE;

#include "Impl/Integer.impl"
