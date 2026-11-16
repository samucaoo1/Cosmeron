#pragma once

#include "Random.space"
#include "Distribution/Distribution.h"
#include "Engine/Engine.h"
#include "Entropy/Entropy.h"
#include "Entropy/Pool/Pool.h"
#include "Mixer/Mixer.h"
#include "Shuffle/Shuffle.h"
#include "Source/Source.h"

#define RANDOM_U64_PROTOTYPE                                                \
  static inline uint64_t RANDOM_FUNC(U64)(void)
#define RANDOM_U64_FROM_SEED_PROTOTYPE                                      \
  static inline uint64_t RANDOM_FUNC(U64_FromSeed)(uint64_t seed)
#define RANDOM_RANGE_U64_PROTOTYPE                                          \
  static inline uint64_t RANDOM_FUNC(RangeU64)(uint64_t minimum,            \
                                                uint64_t maximum)
#define RANDOM_RANGE_I64_PROTOTYPE                                          \
  static inline int64_t RANDOM_FUNC(RangeI64)(int64_t minimum,              \
                                               int64_t maximum)
#define RANDOM_F64_PROTOTYPE                                                \
  static inline double RANDOM_FUNC(F64)(void)
#define RANDOM_BOOL_PROTOTYPE                                               \
  static inline bool RANDOM_FUNC(Bool)(void)

RANDOM_U64_PROTOTYPE;
RANDOM_U64_FROM_SEED_PROTOTYPE;
RANDOM_RANGE_U64_PROTOTYPE;
RANDOM_RANGE_I64_PROTOTYPE;
RANDOM_F64_PROTOTYPE;
RANDOM_BOOL_PROTOTYPE;

#include "Impl/Random.impl"
