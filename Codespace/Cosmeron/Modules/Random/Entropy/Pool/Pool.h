#pragma once

#include "../Entropy.space"

typedef struct RANDOM_ENTROPY_TYPE(Pool) {
  uint64_t state;
  uint64_t count;
} RANDOM_ENTROPY_TYPE(Pool);

inline static void RANDOM_ENTROPY_FUNC(Pool_Init)(RANDOM_ENTROPY_TYPE(Pool) *pool);
inline static void RANDOM_ENTROPY_FUNC(Pool_Add)(RANDOM_ENTROPY_TYPE(Pool) *pool,
                                               uint64_t value);
inline static uint64_t RANDOM_ENTROPY_FUNC(Pool_Finalize)(
    const RANDOM_ENTROPY_TYPE(Pool) *pool);

#include "Impl/Pool.impl"

