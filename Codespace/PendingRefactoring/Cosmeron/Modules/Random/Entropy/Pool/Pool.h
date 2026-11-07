#pragma once

#include "../Entropy.space"

typedef struct RANDOM_ENTROPY_TYPE(Pool) {
  uint64_t state;
  uint64_t count;
} RANDOM_ENTROPY_TYPE(Pool);

#define RANDOM_ENTROPY_POOL_INIT_PROTOTYPE                                   \
  static inline OPSTATUS RANDOM_ENTROPY_FUNC(Pool_Init)(                      \
      RANDOM_ENTROPY_TYPE(Pool) *pool)

#define RANDOM_ENTROPY_POOL_ADD_PROTOTYPE                                    \
  static inline OPSTATUS RANDOM_ENTROPY_FUNC(Pool_Add)(                       \
      RANDOM_ENTROPY_TYPE(Pool) *pool, uint64_t value)

#define RANDOM_ENTROPY_POOL_FINALIZE_PROTOTYPE                               \
  static inline OPSTATUS RANDOM_ENTROPY_FUNC(Pool_Finalize)(                  \
      const RANDOM_ENTROPY_TYPE(Pool) *pool, uint64_t *outValue)

RANDOM_ENTROPY_POOL_INIT_PROTOTYPE;
RANDOM_ENTROPY_POOL_ADD_PROTOTYPE;
RANDOM_ENTROPY_POOL_FINALIZE_PROTOTYPE;

#include "Impl/Pool.impl"

