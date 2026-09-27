#pragma once

#include "Distribution.space"
#include <limits.h>

static inline uint64_t RANDOM_DISTRIBUTION_FUNC(U64)(RANDOM_SOURCE_TYPE(Value) *source,
                                                   uint64_t minimum,
                                                   uint64_t maximum);
static inline int64_t RANDOM_DISTRIBUTION_FUNC(I64)(RANDOM_SOURCE_TYPE(Value) *source,
                                                  int64_t minimum,
                                                  int64_t maximum);

#include "Impl/Integer.impl"
