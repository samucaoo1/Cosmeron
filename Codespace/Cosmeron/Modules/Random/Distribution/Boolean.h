#pragma once

#include "Distribution.space"

#define RANDOM_DISTRIBUTION_BOOL_PROTOTYPE                                   \
  static inline OPSTATUS RANDOM_DISTRIBUTION_FUNC(Bool)(                      \
      RANDOM_SOURCE_TYPE(Value) *source, bool *out)

RANDOM_DISTRIBUTION_BOOL_PROTOTYPE;

#include "Impl/Boolean.impl"
