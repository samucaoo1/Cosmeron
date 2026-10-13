#pragma once

#include "Distribution.space"

#define RANDOM_DISTRIBUTION_F64_PROTOTYPE                                    \
  static inline OPSTATUS RANDOM_DISTRIBUTION_FUNC(F64)(                       \
      RANDOM_SOURCE_TYPE(Value) *source, double *outValue)

RANDOM_DISTRIBUTION_F64_PROTOTYPE;

#include "Impl/Real.impl"
