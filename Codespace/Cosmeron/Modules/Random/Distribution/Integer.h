#pragma once

#include "Distribution.space"
#include <limits.h>

inline static uint64_t RANDOM_DISTRIBUTION_FUNC(U64)(TRandomSource *source,
                                                   uint64_t minimum,
                                                   uint64_t maximum);
inline static int64_t RANDOM_DISTRIBUTION_FUNC(I64)(TRandomSource *source,
                                                  int64_t minimum,
                                                  int64_t maximum);

#include "Impl/Integer.impl"
