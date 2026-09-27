#pragma once

#include "Shuffle.space"

static inline OPSTATUS RANDOM_SHUFFLE_FUNC(VectorFromSource)(
    void *vector,
    size_t count,
    size_t elementSize,
    RANDOM_SOURCE_TYPE(Value) *source);

static inline OPSTATUS RANDOM_SHUFFLE_FUNC(Vector)(void *vector,
                                                 size_t count,
                                                 size_t elementSize);

#define RANDOM_SHUFFLE_VECTOR(VECTOR, COUNT)                                  \
  RANDOM_SHUFFLE_FUNC(Vector)((VECTOR), (COUNT), sizeof(*(VECTOR)))

#define RANDOM_SHUFFLE_VECTOR_FROM_SOURCE(VECTOR, COUNT, SOURCE)              \
  RANDOM_SHUFFLE_FUNC(VectorFromSource)((VECTOR), (COUNT), sizeof(*(VECTOR)),   \
                                      (SOURCE))

#include "Impl/Shuffle.impl"
